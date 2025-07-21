#include "pch.h"
#include "NavMesh.h"
#include "GameObject.h"
#include "ModelRenderer.h"
#include "Model.h"
#include "ModelMesh.h"

NavMesh::NavMesh() : Super(ComponentType::NavMesh)
{
}

NavMesh::~NavMesh()
{
}

void NavMesh::Start()
{
    Super::Start();
    LoadNavMeshData();
    DebugPrintTriangles();
}

void NavMesh::Update()
{
    Super::Update();
}

//void NavMesh::LoadNavMeshData()
//{
//    auto gameObject = GetGameObject();
//    if (!gameObject) return;
//
//    auto modelRenderer = gameObject->GetModelRenderer();
//    if (!modelRenderer) return;
//
//    m_navMeshModel = modelRenderer->GetModel();
//    if (!m_navMeshModel) return;
//
//    // NavMesh 모델에서 삼각형 데이터 추출
//    const auto& meshes = m_navMeshModel->GetMeshes();
//    for (auto& mesh : meshes)
//    {
//        auto geometry = mesh->m_geometry;
//        const auto& vertices = geometry->GetVertices();
//        const auto& indices = geometry->GetIndices();
//
//        // 인덱스 3개씩 묶어서 삼각형 생성
//        for (size_t i = 0; i < indices.size(); i += 3)
//        {
//            NavMeshTriangle triangle;
//
//            for (int j = 0; j < 3; j++)
//            {
//                uint32 index = indices[i + j];
//                triangle.vertices[j] = vertices[index].position;
//            }
//
//            // 삼각형의 법선 벡터 계산
//            Vec3 edge1 = triangle.vertices[1] - triangle.vertices[0];
//            Vec3 edge2 = triangle.vertices[2] - triangle.vertices[0];
//            triangle.normal = edge1.Cross(edge2);
//            triangle.normal.Normalize();
//
//            // 삼각형의 중심점 계산
//            triangle.center = (triangle.vertices[0] + triangle.vertices[1] + triangle.vertices[2]) / 3.0f;
//
//            m_triangles.push_back(triangle);
//        }
//    }
//}

void NavMesh::LoadNavMeshData()
{
    auto gameObject = GetGameObject();
    if (!gameObject) {
        cout << "ERROR: NavMesh GameObject is null!" << endl;
        return;
    }

    auto modelRenderer = gameObject->GetModelRenderer();
    if (!modelRenderer) {
        cout << "ERROR: NavMesh ModelRenderer is null!" << endl;
        return;
    }

    m_navMeshModel = modelRenderer->GetModel();
    if (!m_navMeshModel) {
        cout << "ERROR: NavMesh Model is null!" << endl;
        return;
    }

    const auto& meshes = m_navMeshModel->GetMeshes();
    cout << "NavMesh has " << meshes.size() << " meshes" << endl;

    for (size_t meshIdx = 0; meshIdx < meshes.size(); meshIdx++)
    {
        auto& mesh = meshes[meshIdx];
        auto geometry = mesh->m_geometry;
        if (!geometry) {
            cout << "ERROR: Mesh " << meshIdx << " has null geometry!" << endl;
            continue;
        }

        const auto& vertices = geometry->GetVertices();
        const auto& indices = geometry->GetIndices();

        cout << "Mesh " << meshIdx << ": " << vertices.size() << " vertices, " << indices.size() << " indices" << endl;

        if (vertices.empty() || indices.empty()) continue;

        // Transform 적용
        auto transform = gameObject->GetTransform();
        Matrix worldMatrix = transform->GetWorldMatrix();

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            if (indices[i] >= vertices.size() ||
                indices[i + 1] >= vertices.size() ||
                indices[i + 2] >= vertices.size())
                continue;

            NavMeshTriangle triangle;

            // 월드 좌표로 변환하여 저장
            for (int j = 0; j < 3; j++)
            {
                uint32 index = indices[i + j];
                Vec4 worldPos = Vec4(vertices[index].position.x,
                    vertices[index].position.y,
                    vertices[index].position.z, 1.0f);
                worldPos = XMVector4Transform(worldPos, worldMatrix);
                triangle.vertices[j] = Vec3(worldPos.x, worldPos.y, worldPos.z);
            }

            // 삼각형 유효성 검사
            Vec3 edge1 = triangle.vertices[1] - triangle.vertices[0];
            Vec3 edge2 = triangle.vertices[2] - triangle.vertices[0];
            triangle.normal = edge1.Cross(edge2);

            if (triangle.normal.Length() < 0.001f) continue;

            triangle.normal.Normalize();
            triangle.center = (triangle.vertices[0] + triangle.vertices[1] + triangle.vertices[2]) / 3.0f;

            m_triangles.push_back(triangle);
        }
    }

    cout << "=== NavMesh Load Result ===" << endl;
    cout << "Total triangles loaded: " << m_triangles.size() << endl;

    if (!m_triangles.empty()) {
        cout << "First triangle vertices:" << endl;
        auto& tri = m_triangles[0];
        cout << "  V0: (" << tri.vertices[0].x << ", " << tri.vertices[0].y << ", " << tri.vertices[0].z << ")" << endl;
        cout << "  V1: (" << tri.vertices[1].x << ", " << tri.vertices[1].y << ", " << tri.vertices[1].z << ")" << endl;
        cout << "  V2: (" << tri.vertices[2].x << ", " << tri.vertices[2].y << ", " << tri.vertices[2].z << ")" << endl;

        // NavMesh 범위 계산
        Vec3 minBounds = tri.vertices[0];
        Vec3 maxBounds = tri.vertices[0];

        for (const auto& triangle : m_triangles) {
            for (int i = 0; i < 3; i++) {
                minBounds.x = min(minBounds.x, triangle.vertices[i].x);
                minBounds.y = min(minBounds.y, triangle.vertices[i].y);
                minBounds.z = min(minBounds.z, triangle.vertices[i].z);
                maxBounds.x = max(maxBounds.x, triangle.vertices[i].x);
                maxBounds.y = max(maxBounds.y, triangle.vertices[i].y);
                maxBounds.z = max(maxBounds.z, triangle.vertices[i].z);
            }
        }

        cout << "NavMesh Bounds:" << endl;
        cout << "  Min: (" << minBounds.x << ", " << minBounds.y << ", " << minBounds.z << ")" << endl;
        cout << "  Max: (" << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z << ")" << endl;
    }

    cout << "=== NavMesh Load Result ===" << endl;
    cout << "Total triangles loaded: " << m_triangles.size() << endl;

    // 삼각형 연결성 계산 추가
    if (!m_triangles.empty())
    {
        BuildTriangleConnections();
    }
}


Vec3 NavMesh::GetNearestPointOnNavMesh(const Vec3& worldPos)
{
    if (m_triangles.empty()) return worldPos;

    Vec3 nearestPoint = worldPos;
    float minDistance = FLT_MAX;

    for (const auto& triangle : m_triangles)
    {
        Vec3 projectedPoint = ProjectPointOnTriangle(worldPos, triangle);
        float distance = Vec3::Distance(worldPos, projectedPoint);

        if (distance < minDistance)
        {
            minDistance = distance;
            nearestPoint = projectedPoint;
        }
    }

    return nearestPoint;
}

bool NavMesh::RaycastNavMesh(const Ray& ray, Vec3& hitPoint)
{
    cout << "=== RaycastNavMesh Debug ===" << endl;
    cout << "Triangle count: " << m_triangles.size() << endl;
    cout << "Ray origin: (" << ray.position.x << ", " << ray.position.y << ", " << ray.position.z << ")" << endl;
    cout << "Ray direction: (" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << ")" << endl;

    if (m_triangles.empty())
    {
        cout << "No triangles found!" << endl;
        return false;
    }

    float closestDistance = FLT_MAX;
    bool hit = false;
    int checkedTriangles = 0;

    for (const auto& triangle : m_triangles)
    {
        checkedTriangles++;

        // 삼각형 정보 출력 (처음 몇 개만)
        if (checkedTriangles <= 3)
        {
            cout << "Checking triangle " << checkedTriangles << ":" << endl;
            cout << "  V0: (" << triangle.vertices[0].x << ", " << triangle.vertices[0].y << ", " << triangle.vertices[0].z << ")" << endl;
            cout << "  V1: (" << triangle.vertices[1].x << ", " << triangle.vertices[1].y << ", " << triangle.vertices[1].z << ")" << endl;
            cout << "  V2: (" << triangle.vertices[2].x << ", " << triangle.vertices[2].y << ", " << triangle.vertices[2].z << ")" << endl;
        }

        float distance = 0.0f;

        try
        {
            if (ray.Intersects(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], distance))
            {
                cout << "Hit found! Distance: " << distance << endl;
                if (distance > 0 && distance < closestDistance)
                {
                    closestDistance = distance;
                    hitPoint = ray.position + ray.direction * distance;
                    hit = true;
                    cout << "New closest hit at: (" << hitPoint.x << ", " << hitPoint.y << ", " << hitPoint.z << ")" << endl;
                }
            }
        }
        catch (...)
        {
            cout << "Exception in ray intersection!" << endl;
            continue;
        }
    }

    cout << "Final result: " << (hit ? "HIT" : "MISS") << endl;
    cout << "=========================" << endl;
    return hit;
}
bool NavMesh::IsOnNavMesh(const Vec3& worldPos, float tolerance)
{
    Vec3 nearestPoint = GetNearestPointOnNavMesh(worldPos);
    return Vec3::Distance(worldPos, nearestPoint) <= tolerance;
}

Vec3 NavMesh::ProjectPointOnTriangle(const Vec3& point, const NavMeshTriangle& triangle)
{
    // 삼각형 평면에 점을 투영
    Vec3 v0 = triangle.vertices[0];
    Vec3 planePoint = point - (point - v0).Dot(triangle.normal) * triangle.normal;

    // 투영된 점이 삼각형 내부에 있는지 확인
    if (IsPointInTriangle(planePoint, triangle))
    {
        return planePoint;
    }

    // 삼각형 외부에 있다면 가장 가까운 모서리나 정점으로
    Vec3 closestPoint = triangle.vertices[0];
    float minDist = Vec3::Distance(point, triangle.vertices[0]);

    for (int i = 0; i < 3; i++)
    {
        float dist = Vec3::Distance(point, triangle.vertices[i]);
        if (dist < minDist)
        {
            minDist = dist;
            closestPoint = triangle.vertices[i];
        }
    }

    return closestPoint;
}

bool NavMesh::IsPointInTriangle(const Vec3& point, const NavMeshTriangle& triangle)
{
    Vec3 v0 = triangle.vertices[2] - triangle.vertices[0];
    Vec3 v1 = triangle.vertices[1] - triangle.vertices[0];
    Vec3 v2 = point - triangle.vertices[0];

    float dot00 = v0.Dot(v0);
    float dot01 = v0.Dot(v1);
    float dot02 = v0.Dot(v2);
    float dot11 = v1.Dot(v1);
    float dot12 = v1.Dot(v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

//vector<Vec3> NavMesh::FindPath(const Vec3& start, const Vec3& end)
//{
//    // 간단한 직선 경로 (A* 알고리즘으로 확장 가능)
//    vector<Vec3> path;
//    path.push_back(GetNearestPointOnNavMesh(start));
//    path.push_back(GetNearestPointOnNavMesh(end));
//    return path;
//}

float NavMesh::GetDistance(const Vec3& a, const Vec3& b)
{
    return Vec3::Distance(a, b);
}


// NavMesh 로드 후 상태 확인
void NavMesh::DebugPrintTriangles()
{
    cout << "=== NavMesh Debug Info ===" << endl;
    cout << "Total triangles: " << m_triangles.size() << endl;

    for (size_t i = 0; i < min(m_triangles.size(), (size_t)5); i++)
    {
        auto& tri = m_triangles[i];
        cout << "Triangle " << i << ":" << endl;
        cout << "  V0: (" << tri.vertices[0].x << ", " << tri.vertices[0].y << ", " << tri.vertices[0].z << ")" << endl;
        cout << "  V1: (" << tri.vertices[1].x << ", " << tri.vertices[1].y << ", " << tri.vertices[1].z << ")" << endl;
        cout << "  V2: (" << tri.vertices[2].x << ", " << tri.vertices[2].y << ", " << tri.vertices[2].z << ")" << endl;
    }
}




void NavMesh::BuildTriangleConnections()
{
    cout << "Building triangle connections..." << endl;

    for (size_t i = 0; i < m_triangles.size(); i++)
    {
        m_triangles[i].index = i;
        m_triangles[i].neighbors.clear();

        for (size_t j = 0; j < m_triangles.size(); j++)
        {
            if (i == j) continue;

            if (AreTrianglesAdjacent(m_triangles[i], m_triangles[j]))
            {
                m_triangles[i].neighbors.push_back(j);
            }
        }
    }

    cout << "Triangle connections built. Average neighbors per triangle: "
        << (float)getTotalNeighbors() / m_triangles.size() << endl;
}

bool NavMesh::AreTrianglesAdjacent(const NavMeshTriangle& tri1, const NavMeshTriangle& tri2)
{
    const float EPSILON = 0.01f;
    int sharedVertices = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (Vec3::Distance(tri1.vertices[i], tri2.vertices[j]) < EPSILON)
            {
                sharedVertices++;
                break;
            }
        }
    }

    return sharedVertices >= 2; // 최소 2개의 정점을 공유해야 인접
}

int NavMesh::getTotalNeighbors()
{
    int total = 0;
    for (const auto& tri : m_triangles)
    {
        total += tri.neighbors.size();
    }
    return total;
}

vector<Vec3> NavMesh::FindPath(const Vec3& start, const Vec3& end)
{
    vector<Vec3> path;
    
    // 시작점과 끝점이 포함된 삼각형 찾기
    int startTriangle = FindTriangleContaining(start);
    int endTriangle = FindTriangleContaining(end);
    
    if (startTriangle == -1 || endTriangle == -1)
    {
        cout << "Start or end point not on NavMesh!" << endl;
        path.push_back(GetNearestPointOnNavMesh(start));
        path.push_back(GetNearestPointOnNavMesh(end));
        return path;
    }
    
    // 같은 삼각형에 있으면 직선 경로
    if (startTriangle == endTriangle)
    {
        path.push_back(start);
        path.push_back(end);
        return path;
    }
    
    // A* 알고리즘으로 삼각형 경로 찾기
    vector<int> trianglePath = FindTrianglePath(startTriangle, endTriangle);
    
    if (trianglePath.empty())
    {
        cout << "No path found between triangles!" << endl;
        path.push_back(GetNearestPointOnNavMesh(start));
        path.push_back(GetNearestPointOnNavMesh(end));
        return path;
    }
    
    // 삼각형 경로를 실제 월드 좌표 경로로 변환
    vector<Vec3> rawPath = ConvertTrianglePathToWorldPath(trianglePath, start, end);
    
    // 경로 스무딩 적용
    path = SmoothPath(rawPath);
    
    cout << "Raw path points: " << rawPath.size() << " -> Smoothed points: " << path.size() << endl;
    
    return path;
}

vector<int> NavMesh::FindTrianglePath(int startTriangle, int endTriangle)
{
    vector<PathNode> openList;
    vector<bool> closedList(m_triangles.size(), false);
    vector<PathNode> allNodes(m_triangles.size());

    // 시작 노드 초기화
    PathNode startNode;
    startNode.triangleIndex = startTriangle;
    startNode.position = m_triangles[startTriangle].center;
    startNode.gCost = 0;
    startNode.hCost = Vec3::Distance(m_triangles[startTriangle].center, m_triangles[endTriangle].center);
    startNode.fCost = startNode.gCost + startNode.hCost;
    startNode.parent = -1;

    openList.push_back(startNode);
    allNodes[startTriangle] = startNode;

    while (!openList.empty())
    {
        // fCost가 가장 낮은 노드 선택
        int currentIndex = 0;
        for (size_t i = 1; i < openList.size(); i++)
        {
            if (openList[i].fCost < openList[currentIndex].fCost)
            {
                currentIndex = i;
            }
        }

        PathNode currentNode = openList[currentIndex];
        openList.erase(openList.begin() + currentIndex);
        closedList[currentNode.triangleIndex] = true;

        // 목표에 도달했는지 확인
        if (currentNode.triangleIndex == endTriangle)
        {
            return ReconstructPath(allNodes, endTriangle);
        }

        // 인접한 삼각형들 검사
        for (int neighborIndex : m_triangles[currentNode.triangleIndex].neighbors)
        {
            if (closedList[neighborIndex]) continue;

            float tentativeGCost = currentNode.gCost +
                Vec3::Distance(m_triangles[currentNode.triangleIndex].center,
                    m_triangles[neighborIndex].center);

            // 기존에 더 좋은 경로가 있는지 확인
            bool inOpenList = false;
            for (const auto& node : openList)
            {
                if (node.triangleIndex == neighborIndex)
                {
                    inOpenList = true;
                    if (tentativeGCost >= allNodes[neighborIndex].gCost)
                    {
                        continue; // 더 나쁜 경로이므로 스킵
                    }
                    break;
                }
            }

            // 새로운 노드 생성 또는 업데이트
            PathNode neighborNode;
            neighborNode.triangleIndex = neighborIndex;
            neighborNode.position = m_triangles[neighborIndex].center;
            neighborNode.gCost = tentativeGCost;
            neighborNode.hCost = Vec3::Distance(m_triangles[neighborIndex].center,
                m_triangles[endTriangle].center);
            neighborNode.fCost = neighborNode.gCost + neighborNode.hCost;
            neighborNode.parent = currentNode.triangleIndex;

            allNodes[neighborIndex] = neighborNode;

            if (!inOpenList)
            {
                openList.push_back(neighborNode);
            }
        }
    }

    return vector<int>(); // 경로를 찾을 수 없음
}

vector<int> NavMesh::ReconstructPath(const vector<PathNode>& allNodes, int endTriangle)
{
    vector<int> path;
    int current = endTriangle;

    while (current != -1)
    {
        path.push_back(current);
        current = allNodes[current].parent;
    }

    reverse(path.begin(), path.end());
    return path;
}

int NavMesh::FindTriangleContaining(const Vec3& point)
{
    for (size_t i = 0; i < m_triangles.size(); i++)
    {
        if (IsPointInTriangle(point, m_triangles[i]))
        {
            return i;
        }
    }
    return -1;
}

vector<Vec3> NavMesh::ConvertTrianglePathToWorldPath(const vector<int>& trianglePath,
    const Vec3& start, const Vec3& end)
{
    vector<Vec3> worldPath;

    if (trianglePath.empty()) return worldPath;

    worldPath.push_back(start);

    // 각 삼각형의 중심점을 경유점으로 사용 (더 정교한 방법으로 개선 가능)
    for (size_t i = 1; i < trianglePath.size() - 1; i++)
    {
        worldPath.push_back(m_triangles[trianglePath[i]].center);
    }

    worldPath.push_back(end);

    return worldPath;
}

vector<Vec3> NavMesh::SmoothPath(const vector<Vec3>& originalPath)
{
    if (originalPath.size() <= 2)
    {
        cout << "Path too short for smoothing" << endl;
        return originalPath;
    }

    cout << "Smoothing path with " << originalPath.size() << " points" << endl;

    vector<Vec3> smoothedPath;
    smoothedPath.push_back(originalPath[0]);

    size_t currentIndex = 0;

    while (currentIndex < originalPath.size() - 1)
    {
        size_t farthestIndex = currentIndex + 1;

        // 현재 점에서 가장 멀리 볼 수 있는 점 찾기
        for (size_t i = currentIndex + 2; i < originalPath.size(); i++)
        {
            if (HasLineOfSight(smoothedPath.back(), originalPath[i]))
            {
                farthestIndex = i;
            }
            else
            {
                break; // 더 이상 직진할 수 없음
            }
        }

        // 다음 웨이포인트 추가
        if (farthestIndex < originalPath.size() - 1)
        {
            smoothedPath.push_back(originalPath[farthestIndex]);
            cout << "Added waypoint: (" << originalPath[farthestIndex].x
                << ", " << originalPath[farthestIndex].y
                << ", " << originalPath[farthestIndex].z << ")" << endl;
        }

        currentIndex = farthestIndex;
    }

    // 마지막 점 추가
    smoothedPath.push_back(originalPath.back());

    cout << "Smoothed path has " << smoothedPath.size() << " points" << endl;
    return smoothedPath;
}

bool NavMesh::HasLineOfSight(const Vec3& start, const Vec3& end)
{
    // 두 점이 너무 가까우면 항상 true
    float distance = Vec3::Distance(start, end);
    if (distance < 0.1f) return true;

    return IsLineOnNavMesh(start, end);
}

bool NavMesh::IsLineOnNavMesh(const Vec3& start, const Vec3& end, float stepSize)
{
    Vec3 direction = end - start;
    float distance = direction.Length();

    if (distance < 0.001f) return true;

    direction.Normalize();

    int steps = static_cast<int>(distance / stepSize) + 1;

    for (int i = 1; i < steps; i++)
    {
        float t = static_cast<float>(i) / steps;
        Vec3 checkPoint = start + (end - start) * t;

        // 해당 점이 NavMesh 위에 있는지 확인
        if (!IsOnNavMesh(checkPoint, 1.0f))
        {
            return false;
        }
    }

    return true;
}