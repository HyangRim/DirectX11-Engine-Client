#include "pch.h"
#include "VflyQuadTree.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "BaseCollider.h"
#include "SphereCollider.h"


int VflyQuadTree::s_nextNodeId = 0;

QuadTreeNode::~QuadTreeNode()
{
    // 자동으로 unique_ptr이 정리됨
}

VflyQuadTree::VflyQuadTree(float screenWidth, float screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
    m_root = make_unique<QuadTreeNode>();
    m_root->bounds = { 0, 0, (LONG)screenWidth, (LONG)screenHeight };
    m_root->nodeId = s_nextNodeId++;
}

VflyQuadTree::~VflyQuadTree()
{
    Clear();
}

void VflyQuadTree::Clear()
{
    if (m_root)
    {
        m_root->objects.clear();
        for (int i = 0; i < 4; ++i)
        {
            m_root->children[i].reset();
        }
        m_root->isLeaf = true;
    }
    m_insertedObjects.clear();
    m_stats = QuadTreeStats{};
}

void VflyQuadTree::Insert(shared_ptr<GameObject> object)
{
    if (!object || !object->GetCollider()) return;

    // 중복 검사
    if (m_insertedObjects.find(object) != m_insertedObjects.end())
        return;

    InsertIntoNode(m_root, object, 0);
    m_insertedObjects.insert(object);
}

void VflyQuadTree::Build()
{
    auto start = std::chrono::high_resolution_clock::now();

    // Insert 과정에서 이미 트리가 구성되므로 통계만 업데이트
    UpdateStats();

    auto end = std::chrono::high_resolution_clock::now();
    m_stats.lastBuildTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

void VflyQuadTree::Split(unique_ptr<QuadTreeNode>& node, int depth)
{
    if (!node || !node->isLeaf || depth >= QuadTreeNode::MAX_DEPTH) return;

    LONG halfWidth = (node->bounds.right - node->bounds.left) / 2;
    LONG halfHeight = (node->bounds.bottom - node->bounds.top) / 2;
    LONG x = node->bounds.left;
    LONG y = node->bounds.top;

    // 4개의 자식 노드 생성
    for (int i = 0; i < 4; ++i)
    {
        node->children[i] = make_unique<QuadTreeNode>();
        node->children[i]->nodeId = s_nextNodeId++;
    }

    // 경계 설정
    node->children[0]->bounds = { x, y, x + halfWidth, y + halfHeight }; // 좌상단
    node->children[1]->bounds = { x + halfWidth, y, x + halfWidth * 2, y + halfHeight }; // 우상단
    node->children[2]->bounds = { x, y + halfHeight, x + halfWidth, y + halfHeight * 2 }; // 좌하단
    node->children[3]->bounds = { x + halfWidth, y + halfHeight, x + halfWidth * 2, y + halfHeight * 2 }; // 우하단

    node->isLeaf = false;

    // 기존 객체들을 자식으로 재분배 (개선된 방식: 여러 노드에 중복 삽입 허용)
    vector<shared_ptr<GameObject>> tempObjects = move(node->objects);
    node->objects.clear();

    for (auto& obj : tempObjects)
    {
        shared_ptr<Camera> camera = CURSCENE->GetMainCamera()->GetCamera();
        RECT objBounds = GetObjectScreenBounds(obj, camera);

        // 모든 자식 노드와 교차 검사하여 해당하는 모든 노드에 삽입
        bool insertedIntoChild = false;
        for (int i = 0; i < 4; ++i)
        {
            RECT intersection;
            if (IntersectRect(&intersection, &objBounds, &node->children[i]->bounds))
            {
                InsertIntoNode(node->children[i], obj, depth + 1);
                insertedIntoChild = true;
            }
        }

        // 만약 어떤 자식 노드와도 교차하지 않으면 현재 노드에 보관
        if (!insertedIntoChild)
        {
            node->objects.push_back(obj);
        }
    }
}

void VflyQuadTree::InsertIntoNode(unique_ptr<QuadTreeNode>& node, shared_ptr<GameObject> object, int depth)
{
    if (!node) return;

    if (node->isLeaf)
    {
        // 이미 존재하는지 확인 (중복 방지)
        auto it = find(node->objects.begin(), node->objects.end(), object);
        if (it == node->objects.end())
        {
            node->objects.push_back(object);
        }

        // 분할 조건 확인
        if (node->objects.size() > QuadTreeNode::MAX_OBJECTS && depth < QuadTreeNode::MAX_DEPTH)
        {
            Split(node, depth);
        }
    }
    else
    {
        // 객체가 교차하는 모든 자식 노드에 삽입
        shared_ptr<Camera> camera = CURSCENE->GetMainCamera()->GetCamera();
        RECT objBounds = GetObjectScreenBounds(object, camera);

        bool insertedIntoChild = false;
        for (int i = 0; i < 4; ++i)
        {
            RECT intersection;
            if (IntersectRect(&intersection, &objBounds, &node->children[i]->bounds))
            {
                InsertIntoNode(node->children[i], object, depth + 1);
                insertedIntoChild = true;
            }
        }

        // 어떤 자식 노드와도 교차하지 않으면 현재 노드에 보관
        if (!insertedIntoChild)
        {
            auto it = find(node->objects.begin(), node->objects.end(), object);
            if (it == node->objects.end())
            {
                node->objects.push_back(object);
            }
        }
    }
}

vector<shared_ptr<GameObject>> VflyQuadTree::Query(const Ray& ray, shared_ptr<Camera> camera)
{
    auto start = std::chrono::high_resolution_clock::now();

    vector<shared_ptr<GameObject>> result;
    QueryNode(m_root, ray, camera, result);

    // 중복 제거
    sort(result.begin(), result.end());
    result.erase(unique(result.begin(), result.end()), result.end());

    auto end = std::chrono::high_resolution_clock::now();
    m_stats.lastQueryTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    return result;
}

void VflyQuadTree::QueryNode(const unique_ptr<QuadTreeNode>& node, const Ray& ray, shared_ptr<Camera> camera,
    vector<shared_ptr<GameObject>>& result)
{
    if (!node) return;

    // Ray가 이 노드의 영역과 교차하는지 확인
    if (!RayIntersectsAABB(ray, node->bounds, camera)) return;

    // 현재 노드의 객체들 추가
    for (auto& obj : node->objects)
    {
        if (IsObjectVisible(obj, camera))
        {
            result.push_back(obj);
        }
    }

    // 자식 노드들을 재귀적으로 검사
    if (!node->isLeaf)
    {
        for (int i = 0; i < 4; ++i)
        {
            QueryNode(node->children[i], ray, camera, result);
        }
    }
}
bool VflyQuadTree::RayIntersectsAABB(const Ray& ray, const RECT& rect, shared_ptr<Camera> camera)
{
    // **새로운 방식: 2D 화면 좌표에서 직접 교차 검사**

    // 1. Ray의 시작점과 끝점을 화면 좌표로 변환
    Vec2 rayStart = WorldToScreen(ray.position, camera);
    Vec2 rayEnd = WorldToScreen(ray.position + ray.direction * 1000.0f, camera); // 충분히 먼 거리

    // 2. 화면 밖 Ray 제외
    Viewport viewport = GRAPHICS->GetViewport();
    if ((rayStart.x < -1000 && rayEnd.x < -1000) ||
        (rayStart.x > viewport.GetWidth() + 1000 && rayEnd.x > viewport.GetWidth() + 1000) ||
        (rayStart.y < -1000 && rayEnd.y < -1000) ||
        (rayStart.y > viewport.GetHeight() + 1000 && rayEnd.y > viewport.GetHeight() + 1000))
    {
        return false;
    }

    // 3. 2D 선분-사각형 교차 검사
    return LineIntersectsRect(rayStart, rayEnd, rect);
}

// 2D 선분-사각형 교차 검사 함수 추가
bool VflyQuadTree::LineIntersectsRect(const Vec2& lineStart, const Vec2& lineEnd, const RECT& rect)
{
    // 사각형의 네 모서리와 선분의 교차 검사
    Vec2 rectPoints[4] = {
        Vec2(rect.left, rect.top),      // 좌상단
        Vec2(rect.right, rect.top),     // 우상단
        Vec2(rect.right, rect.bottom),  // 우하단
        Vec2(rect.left, rect.bottom)    // 좌하단
    };

    // 사각형의 네 변과 선분의 교차 검사
    for (int i = 0; i < 4; ++i)
    {
        Vec2 edgeStart = rectPoints[i];
        Vec2 edgeEnd = rectPoints[(i + 1) % 4];

        if (LineSegmentIntersect(lineStart, lineEnd, edgeStart, edgeEnd))
        {
            return true;
        }
    }

    // 선분의 시작점이나 끝점이 사각형 내부에 있는지 검사
    if (PointInRect(lineStart, rect) || PointInRect(lineEnd, rect))
    {
        return true;
    }

    return false;
}

// 두 선분의 교차 검사
bool VflyQuadTree::LineSegmentIntersect(const Vec2& p1, const Vec2& q1, const Vec2& p2, const Vec2& q2)
{
    auto orientation = [](const Vec2& p, const Vec2& q, const Vec2& r) -> int {
        float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (abs(val) < 1e-6f) return 0;  // 평행
        return (val > 0) ? 1 : 2;        // 시계방향 또는 반시계방향
        };

    auto onSegment = [](const Vec2& p, const Vec2& q, const Vec2& r) -> bool {
        return q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
            q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y);
        };

    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // 일반적인 경우
    if (o1 != o2 && o3 != o4)
        return true;

    // 특수한 경우들
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false;
}

// 점이 사각형 내부에 있는지 검사
bool VflyQuadTree::PointInRect(const Vec2& point, const RECT& rect)
{
    return point.x >= rect.left && point.x <= rect.right &&
        point.y >= rect.top && point.y <= rect.bottom;
}

bool VflyQuadTree::IsObjectVisible(shared_ptr<GameObject> object, shared_ptr<Camera> camera)
{
    //Vec3 worldPos = object->GetTransform()->GetPosition();
    //Vec3 cameraPos = camera->GetTransform()->GetPosition();

    //// 거리 기반 컬링
    //float distance = Vec3::Distance(worldPos, cameraPos);
    //if (distance > 300.0f) return false; // 300 -> 500으로 확장

    //// 시야각 기반 컬링
    //Vec3 dirToObj = worldPos - cameraPos;
    //dirToObj.Normalize();
    //Vec3 cameraLook = camera->GetTransform()->GetLook();

    //float dot = dirToObj.Dot(cameraLook);
    //if (dot < -0.5f) return false; // -0.3 -> -0.5로 조정

    //return true;

    Vec3 worldPos = object->GetTransform()->GetPosition();
    Vec3 cameraPos = camera->GetTransform()->GetPosition();

    // 1. 거리 기반 컬링
    float distance = Vec3::Distance(worldPos, cameraPos);
    if (distance > 500.0f) return false;

    // 2. 백페이스 컬링 (카메라 뒤쪽)
    Vec3 dirToObj = worldPos - cameraPos;
    dirToObj.Normalize();
    Vec3 cameraLook = camera->GetTransform()->GetLook();

    float dot = dirToObj.Dot(cameraLook);
    if (dot < 0.1f) return false; // 더 엄격한 조건

    // 3. **추가: 화면 투영 검사**
    Viewport viewport = GRAPHICS->GetViewport();
    Matrix worldMatrix = Matrix::Identity;
    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjectionMatrix();

    Vec3 screenPos = viewport.Project(worldPos, worldMatrix, viewMatrix, projMatrix);

    // 화면 경계 검사 (여유 공간 최소화)
    float margin = 50.0f; // 여유 공간 줄임
    if (screenPos.x < -margin || screenPos.x > viewport.GetWidth() + margin ||
        screenPos.y < -margin || screenPos.y > viewport.GetHeight() + margin ||
        screenPos.z < 0 || screenPos.z > 1)
    {
        return false;
    }

    // 4. **추가: FOV 기반 시야각 검사**
    Vec3 cameraRight = camera->GetTransform()->GetRight();
    Vec3 cameraUp = camera->GetTransform()->GetUp();

    // 카메라 기준 로컬 좌표로 변환
    Vec3 localPos = worldPos - cameraPos;
    float forward = localPos.Dot(cameraLook);
    float right = localPos.Dot(cameraRight);
    float up = localPos.Dot(cameraUp);

    if (forward <= 0.1f) return false; // 너무 가까운 객체 제외

    // FOV 기반 시야각 검사
    float fov = XMConvertToDegrees(camera->GetFOV());
    float aspectRatio = viewport.GetWidth() / viewport.GetHeight();

    float horizontalFOV = fov * aspectRatio;
    float verticalFOV = fov;

    float horizontalAngle = XMConvertToDegrees(atan2(abs(right), forward));
    float verticalAngle = XMConvertToDegrees(atan2(abs(up), forward));

    if (horizontalAngle > horizontalFOV / 2.0f + 10.0f || // 10도 여유
        verticalAngle > verticalFOV / 2.0f + 10.0f)
    {
        return false;
    }

    return true;
}

int VflyQuadTree::FindBestChildren(const unique_ptr<QuadTreeNode>& node, const RECT& objBounds, vector<int>& childrenIndices)
{
    return 0;
}

RECT VflyQuadTree::GetObjectScreenBounds(shared_ptr<GameObject> object, shared_ptr<Camera> camera)
{
    Vec3 worldPos = object->GetTransform()->GetPosition();
    Vec3 cameraPos = camera->GetTransform()->GetPosition();

    // 카메라 뒤에 있는 객체 제외
    Vec3 dirToObj = worldPos - cameraPos;
    Vec3 cameraLook = camera->GetTransform()->GetLook();

    if (dirToObj.Dot(cameraLook) < 0)
    {
        return { -10000, -10000, -10000, -10000 };
    }

    // **수정: 통일된 좌표 변환 사용**
    Vec2 screenCenter = WorldToScreen(worldPos, camera);

    // 화면 밖 객체 제외
    Viewport viewport = GRAPHICS->GetViewport();
    if (screenCenter.x < -100 || screenCenter.x > viewport.GetWidth() + 100 ||
        screenCenter.y < -100 || screenCenter.y > viewport.GetHeight() + 100)
    {
        return { -10000, -10000, -10000, -10000 };
    }

    // **수정: 더 정확한 크기 계산**
    float distance = Vec3::Distance(worldPos, cameraPos);

    // 실제 콜라이더 크기 기반 계산
    float actualRadius = 1.0f;
    if (object->GetCollider())
    {
        // SphereCollider의 경우 실제 반지름 사용
        if (object->GetCollider()->GetColliderType() == ColliderType::Sphere)
        {
            auto sphereCollider = dynamic_pointer_cast<SphereCollider>(object->GetCollider());
            actualRadius = sphereCollider->GetRadius();
        }
    }

    // 거리 기반 스크린 크기 계산
    float screenRadius = max(2.0f, (actualRadius * 100.0f) / distance);

    // 스케일 적용
    Vec3 scale = object->GetTransform()->GetScale();
    float avgScale = (scale.x + scale.y + scale.z) / 3.0f;
    screenRadius *= avgScale;

    // 최대 크기 제한
    screenRadius = min(screenRadius, 20.0f);

    LONG screenX = (LONG)screenCenter.x;
    LONG screenY = (LONG)screenCenter.y;
    LONG size = (LONG)screenRadius;

    return { screenX - size, screenY - size, screenX + size, screenY + size };
}

void VflyQuadTree::UpdateStats()
{
    m_stats = QuadTreeStats{};
    if (m_root)
    {
        CalculateStats(m_root, 0);
        if (m_stats.leafNodes > 0)
        {
            m_stats.avgObjectsPerLeaf = (float)m_stats.totalObjects / m_stats.leafNodes;
        }
    }
}

void VflyQuadTree::CalculateStats(const unique_ptr<QuadTreeNode>& node, int depth)
{
    if (!node) return;

    m_stats.totalNodes++;
    m_stats.maxDepth = max(m_stats.maxDepth, depth);
    m_stats.totalObjects += (int)node->objects.size();

    if (node->isLeaf)
    {
        m_stats.leafNodes++;
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            CalculateStats(node->children[i], depth + 1);
        }
    }
}

// 디버그 함수들 (기존과 동일하지만 성능 정보 추가)
void VflyQuadTree::DebugDraw(shared_ptr<Camera> camera)
{
    if (!m_root) return;

    cout << "=== QuadTree Debug Draw ===" << endl;
    cout << "Screen Size: " << m_screenWidth << " x " << m_screenHeight << endl;
    cout << "Performance Stats:" << endl;
    cout << "  Total Nodes: " << m_stats.totalNodes << endl;
    cout << "  Leaf Nodes: " << m_stats.leafNodes << endl;
    cout << "  Max Depth: " << m_stats.maxDepth << endl;
    cout << "  Avg Objects/Leaf: " << m_stats.avgObjectsPerLeaf << endl;
    cout << "  Last Query Time: " << m_stats.lastQueryTime.count() << "μs" << endl;
    cout << "  Last Build Time: " << m_stats.lastBuildTime.count() << "μs" << endl;

    DebugDrawNode(m_root, 0, camera);
}

void VflyQuadTree::PrintDuplicates()
{
    unordered_map<shared_ptr<GameObject>, int> objectCount;
    CountObjectsInNode(m_root, objectCount);

    cout << "=== 중복 객체 검사 결과 ===" << endl;
    int duplicateCount = 0;
    for (auto& pair : objectCount)
    {
        if (pair.second > 1)
        {
            wcout << L"중복 객체: " << pair.first->GetName()
                << L" (개수: " << pair.second << L")" << endl;
            duplicateCount++;
        }
    }
    cout << "총 중복 객체 수: " << duplicateCount << endl;
}

// 나머지 함수들은 기존과 동일...
void VflyQuadTree::DebugDrawNode(const unique_ptr<QuadTreeNode>& node, int depth, shared_ptr<Camera> camera)
{
    if (!node) return;

    string indent(depth * 2, ' ');
    cout << indent << "Node " << node->nodeId << " [D" << depth << "]: ";
    cout << "Bounds(" << node->bounds.left << "," << node->bounds.top << ","
        << node->bounds.right << "," << node->bounds.bottom << ") ";
    cout << "Objects: " << node->objects.size();

    if (node->isLeaf)
    {
        cout << " [LEAF]";
    }
    else
    {
        cout << " [BRANCH]";
    }
    cout << endl;

    if (!node->objects.empty())
    {
        for (auto& obj : node->objects)
        {
            cout << indent << "  - " << ws2s(obj->GetName()) << endl;
        }
    }

    if (!node->isLeaf)
    {
        for (int i = 0; i < 4; ++i)
        {
            DebugDrawNode(node->children[i], depth + 1, camera);
        }
    }
}

void VflyQuadTree::PrintTreeStructure()
{
    if (!m_root)
    {
        cout << "QuadTree is empty!" << endl;
        return;
    }

    cout << "=== QuadTree Structure ===" << endl;
    PrintNodeStructure(m_root, 0, "");
}

void VflyQuadTree::PrintNodeStructure(const unique_ptr<QuadTreeNode>& node, int depth, const string& prefix)
{
    if (!node) return;

    cout << prefix;
    cout << "├─ [N" << node->nodeId << "|D" << depth << "] ";
    cout << "(" << node->bounds.left << "," << node->bounds.top << ","
        << node->bounds.right << "," << node->bounds.bottom << ") ";
    cout << "Objs:" << node->objects.size();

    if (node->isLeaf)
    {
        cout << " LEAF";
    }
    cout << endl;

    if (!node->isLeaf)
    {
        string newPrefix = prefix + "│  ";
        for (int i = 0; i < 4; ++i)
        {
            PrintNodeStructure(node->children[i], depth + 1, newPrefix);
        }
    }
}

void VflyQuadTree::GetNodeBounds(vector<RECT>& bounds, vector<int>& depths)
{
    bounds.clear();
    depths.clear();

    if (m_root)
    {
        CollectNodeBounds(m_root, 0, bounds, depths);
    }
}

void VflyQuadTree::CollectNodeBounds(const unique_ptr<QuadTreeNode>& node, int depth,
    vector<RECT>& bounds, vector<int>& depths)
{
    if (!node) return;

    bounds.push_back(node->bounds);
    depths.push_back(depth);

    if (!node->isLeaf)
    {
        for (int i = 0; i < 4; ++i)
        {
            CollectNodeBounds(node->children[i], depth + 1, bounds, depths);
        }
    }
}

void VflyQuadTree::CountObjectsInNode(const unique_ptr<QuadTreeNode>& node, unordered_map<shared_ptr<GameObject>, int>& objectCount)
{
    if (!node) return;

    for (auto& obj : node->objects)
    {
        objectCount[obj]++;
    }

    if (!node->isLeaf)
    {
        for (int i = 0; i < 4; ++i)
        {
            CountObjectsInNode(node->children[i], objectCount);
        }
    }
}

string VflyQuadTree::ws2s(const wstring& wstr)
{
    string str;
    str.assign(wstr.begin(), wstr.end());
    return str;
}

void VflyQuadTree::PrintObjectLocation(shared_ptr<GameObject> targetObject)
{
    cout << "=== 객체 위치 분석: " << ws2s(targetObject->GetName()) << " ===" << endl;

    shared_ptr<Camera> camera = CURSCENE->GetMainCamera()->GetCamera();
    RECT objBounds = GetObjectScreenBounds(targetObject, camera);

    cout << "객체 화면 좌표: (" << objBounds.left << ", " << objBounds.top
        << ", " << objBounds.right << ", " << objBounds.bottom << ")" << endl;

    PrintObjectInNodes(m_root, targetObject, 0);
}

void VflyQuadTree::PrintObjectInNodes(const unique_ptr<QuadTreeNode>& node, shared_ptr<GameObject> targetObject, int depth)
{
    if (!node) return;

    // 이 노드에 객체가 있는지 확인
    auto it = find(node->objects.begin(), node->objects.end(), targetObject);
    if (it != node->objects.end())
    {
        string indent(depth * 2, ' ');
        cout << indent << "발견됨 - 노드 " << node->nodeId << " [깊이 " << depth << "] ";
        cout << "경계: (" << node->bounds.left << ", " << node->bounds.top
            << ", " << node->bounds.right << ", " << node->bounds.bottom << ")" << endl;

        // 노드 타입 표시
        if (depth == 1) // 루트의 자식 노드들
        {
            string quadrant = "";
            switch (node->nodeId % 4) // 간단한 사분면 판별
            {
            case 0: quadrant = "좌상단"; break;
            case 1: quadrant = "우상단"; break;
            case 2: quadrant = "좌하단"; break;
            case 3: quadrant = "우하단"; break;
            }
            cout << indent << "  -> " << quadrant << " 사분면" << endl;
        }
    }

    // 자식 노드들 검사
    if (!node->isLeaf)
    {
        for (int i = 0; i < 4; ++i)
        {
            PrintObjectInNodes(node->children[i], targetObject, depth + 1);
        }
    }
}

Vec2 VflyQuadTree::WorldToScreen(const Vec3& worldPos, shared_ptr<Camera> camera)
{
    Viewport viewport = GRAPHICS->GetViewport();
    Matrix worldMatrix = Matrix::Identity;
    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjectionMatrix();

    // 월드 좌표를 화면 좌표로 변환
    Vec3 screenPos = viewport.Project(worldPos, worldMatrix, viewMatrix, projMatrix);

    return Vec2(screenPos.x, screenPos.y);
}

Vec3 VflyQuadTree::ScreenToWorld(const Vec2& screenPos, shared_ptr<Camera> camera, float depth)
{
    Viewport viewport = GRAPHICS->GetViewport();
    Matrix worldMatrix = Matrix::Identity;
    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjectionMatrix();

    // 화면 좌표를 월드 좌표로 변환
    Vec3 screenPos3D = Vec3(screenPos.x, screenPos.y, depth);
    Vec3 worldPos = viewport.UnProject(screenPos3D, worldMatrix, viewMatrix, projMatrix);

    return worldPos;
}

// VflyQuadTree.cpp에 추가
void VflyQuadTree::DebugCoordinateTransform(const Vec2& mousePos, shared_ptr<Camera> camera)
{
    cout << "=== 좌표 변환 디버깅 ===" << endl;
    cout << "마우스 좌표: (" << mousePos.x << ", " << mousePos.y << ")" << endl;

    // 마우스 위치의 월드 좌표 계산
    Vec3 worldPos = ScreenToWorld(mousePos, camera, 0.5f);
    cout << "마우스 월드 좌표: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")" << endl;

    // 다시 화면 좌표로 변환
    Vec2 backToScreen = WorldToScreen(worldPos, camera);
    cout << "역변환 화면 좌표: (" << backToScreen.x << ", " << backToScreen.y << ")" << endl;

    float error = sqrt(pow(mousePos.x - backToScreen.x, 2) + pow(mousePos.y - backToScreen.y, 2));
    cout << "변환 오차: " << error << " 픽셀" << endl;
}