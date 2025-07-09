#include "pch.h"
#include "VflyQuadTree.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

#include "BaseCollider.h"
#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"

QuadTreeNode::~QuadTreeNode() 
{
    
}

VflyQuadTree::VflyQuadTree(float screenWidth, float screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight) 
{
    m_root = make_unique<QuadTreeNode>();
    m_root->bounds = { 0, 0, (LONG)screenWidth, (LONG)screenHeight };
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
    // Insert 과정에서 이미 트리가 구성되므로 별도 작업 불필요
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
    }

    // 경계 설정
    node->children[0]->bounds = { x, y, x + halfWidth, y + halfHeight }; // 좌상단
    node->children[1]->bounds = { x + halfWidth, y, x + halfWidth * 2, y + halfHeight }; // 우상단
    node->children[2]->bounds = { x, y + halfHeight, x + halfWidth, y + halfHeight * 2 }; // 좌하단
    node->children[3]->bounds = { x + halfWidth, y + halfHeight, x + halfWidth * 2, y + halfHeight * 2 }; // 우하단

    node->isLeaf = false;

    // 기존 객체들을 자식으로 재분배 (중복 제거)
    vector<shared_ptr<GameObject>> tempObjects = move(node->objects);
    node->objects.clear();

    for (auto& obj : tempObjects)
    {
        InsertIntoNode(node, obj, depth); // 재귀 호출로 적절한 위치에 배치
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
        // 객체가 완전히 포함되는 가장 적합한 자식 노드 찾기
        shared_ptr<Camera> camera = CURSCENE->GetMainCamera()->GetCamera();
        RECT objBounds = GetObjectScreenBounds(object, camera);

        int bestChild = FindBestChild(node, objBounds);

        if (bestChild != -1) 
        {
            // 하나의 자식에만 삽입
            InsertIntoNode(node->children[bestChild], object, depth + 1);
        }
        else 
        {
            // 걸쳐있으면 현재 노드에 보관 (중복 방지)
            auto it = find(node->objects.begin(), node->objects.end(), object);
            if (it == node->objects.end()) 
            {
                node->objects.push_back(object);
            }
        }
    }
}

int VflyQuadTree::FindBestChild(const unique_ptr<QuadTreeNode>& node, const RECT& objBounds)
{
    int bestChild = -1;
    int intersectionCount = 0;

    for (int i = 0; i < 4; ++i) 
    {
        RECT intersection;
        if (IntersectRect(&intersection, &objBounds, &node->children[i]->bounds)) 
        {
            intersectionCount++;
            bestChild = i;
        }
    }

    // 하나의 자식에만 완전히 포함되는 경우만 반환
    return (intersectionCount == 1) ? bestChild : -1;
}

vector<shared_ptr<GameObject>> VflyQuadTree::Query(const Ray& ray, shared_ptr<Camera> camera)
{
    vector<shared_ptr<GameObject>> result;
    QueryNode(m_root, ray, camera, result);
    return result;
}

void VflyQuadTree::QueryNode(const unique_ptr<QuadTreeNode>& node, const Ray& ray, shared_ptr<Camera> camera,
    vector<shared_ptr<GameObject>>& result) 
{
    if (!node) return;

    // Ray가 이 노드의 영역과 교차하는지 확인
    if (!RayIntersectsRect(ray, node->bounds, camera)) return;

    if (node->isLeaf) 
    {
        // 리프 노드면 모든 오브젝트를 결과에 추가
        for (auto& obj : node->objects)
        {
            result.push_back(obj);
        }
    }
    else 
    {
        // 현재 노드의 객체들 추가 (경계에 걸친 객체들)
        for (auto& obj : node->objects) 
        {
            result.push_back(obj);
        }

        // 자식 노드들을 재귀적으로 검사
        for (int i = 0; i < 4; ++i) 
        {
            QueryNode(node->children[i], ray, camera, result);
        }
    }
}

bool VflyQuadTree::RayIntersectsRect(const Ray& ray, const RECT& rect, shared_ptr<Camera> camera)
{
    // 화면 좌표계의 사각형을 월드 좌표계로 변환하여 Ray와 교차 검사
    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjectionMatrix();

    float width = GRAPHICS->GetViewport().GetWidth();
    float height = GRAPHICS->GetViewport().GetHeight();

    // 화면 좌표를 NDC로 변환
    float left = (+2.0f * rect.left / width - 1.0f) / projMatrix(0, 0);
    float right = (+2.0f * rect.right / width - 1.0f) / projMatrix(0, 0);
    float top = (-2.0f * rect.top / height + 1.0f) / projMatrix(1, 1);
    float bottom = (-2.0f * rect.bottom / height + 1.0f) / projMatrix(1, 1);

    // 간단한 교차 검사: Ray의 방향이 사각형 영역을 지나가는지 확인
    Vec4 rayDir = Vec4(ray.direction.x, ray.direction.y, ray.direction.z, 0.f);
    Vec3 viewRayDir = XMVector3TransformNormal(rayDir, viewMatrix);

    // View 공간에서 Ray가 사각형 영역과 교차하는지 확인
    if (viewRayDir.x != 0)
    {
        float t1 = left / viewRayDir.x;
        float t2 = right / viewRayDir.x;
        if (t1 > t2) swap(t1, t2);
        if (t2 < 0) return false;
    }

    if (viewRayDir.y != 0) 
    {
        float t1 = bottom / viewRayDir.y;
        float t2 = top / viewRayDir.y;
        if (t1 > t2) swap(t1, t2);
        if (t2 < 0) return false;
    }

    return true;
}
//
//RECT VflyQuadTree::GetObjectScreenBounds(shared_ptr<GameObject> object, shared_ptr<Camera> camera) {
//    // 오브젝트의 월드 좌표를 화면 좌표로 변환
//    Vec3 worldPos = object->GetTransform()->GetPosition();
//
//    Matrix viewMatrix = camera->GetViewMatrix();
//    Matrix projMatrix = camera->GetProjectionMatrix();
//    Matrix wvp = Matrix::Identity * viewMatrix * projMatrix;
//
//    Vec3 screenPos = Vec3::Transform(worldPos, wvp);
//
//    float width = GRAPHICS->GetViewport().GetWidth();
//    float height = GRAPHICS->GetViewport().GetHeight();
//
//    LONG screenX = (LONG)((screenPos.x + 1.0f) * (width / 2));
//    LONG screenY = (LONG)((-screenPos.y + 1.0f) * (height / 2));
//
//    // 오브젝트 크기를 고려한 바운딩 박스 (콜라이더 크기 기반으로 개선 가능)
//    LONG size = 50; // 임시 크기, 실제로는 콜라이더 크기 사용
//
//    return { screenX - size, screenY - size, screenX + size, screenY + size };
//}

bool VflyQuadTree::Contains(shared_ptr<GameObject> object)
{
    return ContainsInNode(m_root, object);
}

bool VflyQuadTree::ContainsInNode(const unique_ptr<QuadTreeNode>& node, shared_ptr<GameObject> object) 
{
    if (!node) return false;

    // 현재 노드에서 검사
    auto it = find(node->objects.begin(), node->objects.end(), object);
    if (it != node->objects.end())
    {
        return true;
    }

    // 자식 노드들에서 재귀 검사
    if (!node->isLeaf) 
    {
        for (int i = 0; i < 4; ++i)
        {
            if (ContainsInNode(node->children[i], object))
            {
                return true;
            }
        }
    }

    return false;
}

void VflyQuadTree::PrintDuplicates() 
{
    unordered_map<shared_ptr<GameObject>, int> objectCount;
    CountObjectsInNode(m_root, objectCount);

    cout << "=== 중복 객체 검사 결과 ===" << endl;
    for (auto& pair : objectCount)
    {
        if (pair.second > 1) 
        {
            wcout << L"중복 객체: " << pair.first->GetName()
                << L" (개수: " << pair.second << L")" << endl;
        }
    }
}

void VflyQuadTree::CountObjectsInNode(const unique_ptr<QuadTreeNode>& node,
    unordered_map<shared_ptr<GameObject>, int>& objectCount)
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

RECT VflyQuadTree::GetObjectScreenBounds(shared_ptr<GameObject> object, shared_ptr<Camera> camera)
{
    Vec3 worldPos = object->GetTransform()->GetPosition();
    Vec3 cameraPos = camera->GetTransform()->GetPosition();

    // 카메라 뒤에 있는 객체 제외
    Vec3 dirToObj = worldPos - cameraPos;
    Vec3 cameraLook = camera->GetTransform()->GetLook();

    if (dirToObj.Dot(cameraLook) < 0)
    {
        return { -10000, -10000, -10000, -10000 }; // 화면 완전히 밖
    }

    // 뷰포트 변환
    Viewport viewport = GRAPHICS->GetViewport();
    Matrix worldMatrix = Matrix::Identity;
    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjectionMatrix();

    // 실제 뷰포트 투영 사용
    Vec3 screenPos = viewport.Project(worldPos, worldMatrix, viewMatrix, projMatrix);

    // 화면 밖 객체 제외
    if (screenPos.x < 0 || screenPos.x > viewport.GetWidth() ||
        screenPos.y < 0 || screenPos.y > viewport.GetHeight() ||
        screenPos.z < 0 || screenPos.z > 1) 
    {
        return { -10000, -10000, -10000, -10000 };
    }

    // 거리 기반 크기 계산
    float distance = Vec3::Distance(worldPos, cameraPos);
    float screenRadius = max(5.0f, 50.0f / (distance * 0.1f)); // 거리에 반비례

    LONG screenX = (LONG)screenPos.x;
    LONG screenY = (LONG)screenPos.y;
    LONG size = (LONG)screenRadius;

    return { screenX - size, screenY - size, screenX + size, screenY + size };
}


void VflyQuadTree::DebugDraw(shared_ptr<Camera> camera) 
{
    if (!m_root) return;

    cout << "=== QuadTree Debug Draw ===" << endl;
    cout << "Screen Size: " << m_screenWidth << " x " << m_screenHeight << endl;

    DebugDrawNode(m_root, 0, camera);
}

void VflyQuadTree::DebugDrawNode(const unique_ptr<QuadTreeNode>& node, int depth, shared_ptr<Camera> camera)
{
    if (!node) return;

    // 노드 정보 출력
    string indent(depth * 2, ' ');
    cout << indent << "Depth " << depth << ": ";
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

    // 이 노드에 속한 객체들 나열
    if (!node->objects.empty()) 
    {
        for (auto& obj : node->objects) 
        {
            cout << indent << "  - " << ws2s(obj->GetName()) << endl;
        }
    }

    // 자식 노드들 재귀 처리
    if (!node->isLeaf) 
    {
        for (int i = 0; i < 4; ++i) 
        {
            cout << indent << "Child " << i << " (";
            switch (i)
            {
                case 0: cout << "Top-Left"; break;
                case 1: cout << "Top-Right"; break;
                case 2: cout << "Bottom-Left"; break;
                case 3: cout << "Bottom-Right"; break;
            }
            cout << "):" << endl;
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
    cout << "├─ [D" << depth << "] ";
    cout << "(" << node->bounds.left << "," << node->bounds.top << ","
        << node->bounds.right << "," << node->bounds.bottom << ") ";
    cout << "Objs:" << node->objects.size();

    if (node->isLeaf) 
    {
        cout << " LEAF";
    }
    cout << endl;

    // 자식 노드들 출력
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

//wstring을 string으로 변환
string VflyQuadTree::ws2s(const wstring& wstr) 
{
    string str;
    str.assign(wstr.begin(), wstr.end());
    return str;
}