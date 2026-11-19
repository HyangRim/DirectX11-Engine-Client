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
    InitializeSpatialGrid();
    //DebugPrintTriangles();
}

void NavMesh::Update()
{
    Super::Update();
}

void NavMesh::LoadNavMeshData()
{
    auto gameObject = GetGameObject();
    if (!gameObject) return;

    auto modelRenderer = gameObject->GetModelRenderer();
    if (!modelRenderer) return;

    m_navMeshModel = modelRenderer->GetModel();
    if (!m_navMeshModel) return;

    const auto& meshes = m_navMeshModel->GetMeshes();
    auto transform = gameObject->GetTransform();
    Matrix worldMatrix = transform->GetWorldMatrix();

    // 각 메시에서 삼각형 데이터 추출
    for (size_t meshIdx = 0; meshIdx < meshes.size(); meshIdx++)
    {
        auto& mesh = meshes[meshIdx];
        auto geometry = mesh->m_geometry;
        if (!geometry) continue;

        const auto& vertices = geometry->GetVertices();
        const auto& indices = geometry->GetIndices();
        if (vertices.empty() || indices.empty()) continue;

        // 인덱스 3개씩 처리하여 삼각형 생성
        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            if (indices[i] >= vertices.size() ||
                indices[i + 1] >= vertices.size() ||
                indices[i + 2] >= vertices.size()) continue;

            NavMeshTriangle triangle;

            // 월드 좌표로 변환
            for (int j = 0; j < 3; j++)
            {
                uint32 index = indices[i + j];
                Vec4 localPos = Vec4(vertices[index].position.x, vertices[index].position.y,
                    vertices[index].position.z, 1.0f);
                Vec4 worldPos = XMVector4Transform(localPos, worldMatrix);
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

    if (!m_triangles.empty())
    {
        BuildTriangleConnections();
        BuildEdgeNeighbors();
    }
}

Vec3 NavMesh::GetNearestPointOnNavMesh(const Vec3& worldPos)
{
    if (m_triangles.empty()) return worldPos;

    Vec3 nearestPoint = worldPos;
    float minDistance = FLT_MAX;

    auto key = m_spatialGrid.GetKey(worldPos);
    auto it = m_spatialGrid.grid.find(key);

    if (it != m_spatialGrid.grid.end()) {
        for (int idx : it->second) {
            Vec3 projectedPoint = ProjectPointOnTriangle(worldPos, m_triangles[idx]);
            float distance = GetDistance(worldPos, projectedPoint);

            if (distance < minDistance)
            {
                minDistance = distance;
                nearestPoint = projectedPoint;
            }
        }
    }
    //전수 조사 방식. 
    /*for (const auto& triangle : m_triangles)
    {
        Vec3 projectedPoint = ProjectPointOnTriangle(worldPos, triangle);
        float distance = GetDistance(worldPos, projectedPoint);

        if (distance < minDistance)
        {
            minDistance = distance;
            nearestPoint = projectedPoint;
        }
    }*/

    return nearestPoint;
}

bool NavMesh::RaycastNavMesh(const Ray& ray, Vec3& hitPoint)
{
    if (m_triangles.empty()) return false;

    float closestDistance = FLT_MAX;
    bool hit = false;

    for (const auto& triangle : m_triangles)
    {
        float distance = 0.0f;
        if (ray.Intersects(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], distance))
        {
            if (distance > 0 && distance < closestDistance)
            {
                closestDistance = distance;
                hitPoint = ray.position + ray.direction * distance;
                hit = true;
            }
        }
    }

    return hit;
}

bool NavMesh::IsOnNavMesh(const Vec3& worldPos, float tolerance)
{
    Vec3 nearestPoint = GetNearestPointOnNavMesh(worldPos);
    return GetDistance(worldPos, nearestPoint) <= tolerance;
}

namespace
{
    double NavNowUs()
    {
        static LARGE_INTEGER f = []() { LARGE_INTEGER x; ::QueryPerformanceFrequency(&x); return x; }();
        LARGE_INTEGER c; ::QueryPerformanceCounter(&c);
        return static_cast<double>(c.QuadPart) * 1'000'000.0 / static_cast<double>(f.QuadPart);
    }
}

void NavMesh::FindPath(const Vec3& start, const Vec3& end, vector<Vec3>& outPath)
{
    outPath.clear(); // 기존 내용 제거
    m_lastAStarUs = 0.0;
    m_lastSmoothUs = 0.0;
    int startTriangle = FindTriangleContaining(start);
    int endTriangle = FindTriangleContaining(end);

    // 진단 정보 기록 시작
    m_lastPathDebug = NavPathDebugInfo();
    m_lastPathDebug.start = start;
    m_lastPathDebug.end = end;
    m_lastPathDebug.startTriangle = startTriangle;
    m_lastPathDebug.endTriangle = endTriangle;

    // NavMesh 위에 없는 점들을 가장 가까운 점으로 보정
    if (startTriangle == -1 || endTriangle == -1)
    {
        outPath.reserve(2); // 미리 메모리 할당
        outPath.push_back(GetNearestPointOnNavMesh(start));
        outPath.push_back(GetNearestPointOnNavMesh(end));

        m_lastPathDebug.branch = NavPathBranch::TriangleNotFound;
        if (startTriangle == -1) m_lastPathDebug.bruteStartTriangle = FindTriangleBruteForce(start);
        if (endTriangle == -1) m_lastPathDebug.bruteEndTriangle = FindTriangleBruteForce(end);
        m_lastPathDebug.finalPath = outPath;
        return;
    }

    // 같은 삼각형에 있으면 직선 경로
    if (startTriangle == endTriangle)
    {
        outPath.reserve(2);
        outPath.push_back(start);
        outPath.push_back(end);

        m_lastPathDebug.branch = NavPathBranch::SameTriangle;
        m_lastPathDebug.finalPath = outPath;
        return;
    }

    // A* 알고리즘으로 삼각형 경로 찾기
    const double astarT0 = NavNowUs();
    vector<int> trianglePath;
    if (!FindTrianglePath(startTriangle, endTriangle, trianglePath))
    {
        outPath.reserve(2);
        outPath.push_back(GetNearestPointOnNavMesh(start));
        outPath.push_back(GetNearestPointOnNavMesh(end));

        m_lastPathDebug.branch = NavPathBranch::AStarFailed;
        m_lastPathDebug.finalPath = outPath;
        return;
    }

    // 삼각형 경로를 월드 좌표 경로로 변환
    // rawPath[k] 는 trianglePath[k] 삼각형 안의 점 (0 = start, 마지막 = end, 나머지 = 중심점)
    vector<Vec3> rawPath;
    ConvertTrianglePathToWorldPath(trianglePath, start, end, rawPath);
    m_lastAStarUs = NavNowUs() - astarT0;
    const double smoothT0 = NavNowUs();   // 스무딩 구간 (통로 표시/원복 포함)
    // 스무딩 적용
    //auto stime = std::chrono::high_resolution_clock::now();
    // 통로 표시 (Corridor 판정에 사용) → 요청 끝에 원복
    if (m_corridorMask.size() != m_triangles.size())
        m_corridorMask.assign(m_triangles.size(), 0);
    for (int tri : trianglePath)
        m_corridorMask[tri] = 1;

    vector<Vec3> smoothed;
    vector<int> smoothedTris;   // Corridor 스무딩 결과 점이 든 삼각형
    switch (m_smoothingMode)
    {
    case NavSmoothingMode::Funnel:
    {
        vector<pair<Vec3, Vec3>> portals;
        BuildPortals(trianglePath, start, end, portals);
        StringPull(portals, smoothed);
        m_lastPathDebug.portals = portals;

        break;
    }
    case NavSmoothingMode::FunnelFan:
    {
        vector<pair<Vec3, Vec3>> portals;
        SmoothPathFunnelFan(trianglePath, start, end, smoothed, portals);
        m_lastPathDebug.portals = portals;
        break;
    }
    case NavSmoothingMode::FunnelCurve:
    {
        vector<pair<Vec3, Vec3>> portals;
        SmoothPathFunnelCurve(trianglePath, start, end, smoothed, portals);
        m_lastPathDebug.portals = portals;
        break;
    }
    case NavSmoothingMode::FunnelCurveAngle:
    {
        vector<pair<Vec3, Vec3>> portals;
        SmoothPathFunnelCurveAngle(trianglePath, start, end, smoothed, portals);
        m_lastPathDebug.portals = portals;
        break;
    }
    case NavSmoothingMode::FunnelCurveTangent:
    {
        vector<pair<Vec3, Vec3>> portals;
        SmoothPathFunnelCurveTangent(trianglePath, start, end, smoothed, portals);
        m_lastPathDebug.portals = portals;
        break;
    }
    case NavSmoothingMode::Corridor:
        SmoothPathCorridor(rawPath, trianglePath, smoothed, smoothedTris);
        break;
    case NavSmoothingMode::CorridorPull:
        SmoothPathCorridor(rawPath, trianglePath, smoothed, smoothedTris);
        PullCornersToWall(trianglePath, smoothed, smoothedTris);
        break;
    case NavSmoothingMode::CorridorDP:
        SmoothPathCorridorDP(rawPath, trianglePath, smoothed, smoothedTris);
        break;
    case NavSmoothingMode::CorridorSplit:
        SmoothPathCorridorSplit(rawPath, trianglePath, smoothed, smoothedTris);
        break;
    case NavSmoothingMode::CorridorFan:
        SmoothPathCorridorSplit(rawPath, trianglePath, smoothed, smoothedTris, true);
        break;
    case NavSmoothingMode::CorridorFanPortal:
        SmoothPathCorridorFanPortal(rawPath, trianglePath, smoothed, smoothedTris);
        break;
    default:
        SmoothPath(rawPath, smoothed);
        break;
    }

    outPath = smoothed;

    for (int tri : trianglePath)
        m_corridorMask[tri] = 0;
    m_lastSmoothUs = NavNowUs() - smoothT0;

    m_lastPathDebug.smoothingMode = m_smoothingMode;
    m_lastPathDebug.branch = NavPathBranch::AStarSmoothed;
    m_lastPathDebug.rawPath = rawPath;
    m_lastPathDebug.finalPath = outPath;
    //auto etime = std::chrono::high_resolution_clock::now();
    //cout << std::chrono::duration_cast<std::chrono::microseconds>(etime - stime).count() << "\n";
}


bool NavMesh::FindTrianglePath(int startTriangle, int endTriangle, vector<int>& outPath)
{
    outPath.clear();

    priority_queue<PathNode> openList;
    vector<bool> closedList(m_triangles.size(), false);
    vector<PathNode> allNodes(m_triangles.size());

    // 시작 노드 초기화
    PathNode startNode;
    startNode.triangleIndex = startTriangle;
    startNode.position = m_triangles[startTriangle].center;
    startNode.gCost = 0;
    startNode.hCost = GetDistance(m_triangles[startTriangle].center, m_triangles[endTriangle].center);
    startNode.fCost = startNode.gCost + startNode.hCost;
    startNode.parent = -1;

    openList.push(startNode);
    allNodes[startTriangle] = startNode;

    while (!openList.empty())
    {
        PathNode currentNode = openList.top();
        openList.pop();

        if (closedList[currentNode.triangleIndex]) continue;
        closedList[currentNode.triangleIndex] = true;

        if (currentNode.triangleIndex == endTriangle)
        {
            ReconstructPath(allNodes, endTriangle, outPath);
            return true; // 성공
        }

        // 인접 삼각형들 검사
        for (int neighborIndex : m_triangles[currentNode.triangleIndex].neighbors)
        {
            if (closedList[neighborIndex]) continue;

            float tentativeGCost = currentNode.gCost +
                GetDistance(m_triangles[currentNode.triangleIndex].center, m_triangles[neighborIndex].center);

            if (allNodes[neighborIndex].triangleIndex == -1 || tentativeGCost < allNodes[neighborIndex].gCost)
            {
                PathNode neighborNode;
                neighborNode.triangleIndex = neighborIndex;
                neighborNode.position = m_triangles[neighborIndex].center;
                neighborNode.gCost = tentativeGCost;
                neighborNode.hCost = GetDistance(m_triangles[neighborIndex].center, m_triangles[endTriangle].center);
                neighborNode.fCost = neighborNode.gCost + neighborNode.hCost;
                neighborNode.parent = currentNode.triangleIndex;

                allNodes[neighborIndex] = neighborNode;
                openList.push(neighborNode);
            }
        }
    }

    return false; // 경로를 찾을 수 없음
}

void NavMesh::ReconstructPath(const vector<PathNode>& allNodes, int endTriangle, vector<int>& outPath)
{
    outPath.clear();

    // 역방향으로 경로 구성
    vector<int> reversePath;
    int current = endTriangle;

    while (current != -1)
    {
        reversePath.push_back(current);
        current = allNodes[current].parent;
    }

    // 정방향으로 복사 (메모리 미리 할당)
    outPath.reserve(reversePath.size());
    for (auto it = reversePath.rbegin(); it != reversePath.rend(); ++it)
    {
        outPath.push_back(*it);
    }
}

void NavMesh::ConvertTrianglePathToWorldPath(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath)
{
    outPath.clear();
    if (trianglePath.empty()) return;

    // 예상 크기로 메모리 미리 할당
    outPath.reserve(trianglePath.size() + 1);

    outPath.push_back(start);

    for (size_t i = 1; i < trianglePath.size() - 1; i++)
    {
        outPath.push_back(m_triangles[trianglePath[i]].center);
    }

    outPath.push_back(end);
}

void NavMesh::SmoothPath(const vector<Vec3>& originalPath, vector<Vec3>& outPath)
{
    outPath.clear();
    if (originalPath.size() <= 2)
    {
        outPath = originalPath; // 작은 경우에만 복사
        return;
    }

    // 예상 크기로 메모리 미리 할당 (원본보다 작을 것으로 예상)
    outPath.reserve(originalPath.size());
    outPath.push_back(originalPath[0]);

    const float minDist = 3.0f, maxDist = 10.0f;
    size_t cur = 0;

    while (cur + 1 < originalPath.size())
    {
        int bestNext = cur + 1;
        bool foundValidPath = false;

        Vec3 curPos = outPath.back();
        vector<size_t> candidates;
        candidates.reserve(10); // 일반적인 후보 개수 예상

        // 시야 확보된 모든 지점 수집
        for (size_t i = cur + 1; i < originalPath.size(); ++i)
        {
            bool hasLOS = HasLineOfSight(curPos, originalPath[i]);

            if (hasLOS) {
                bestNext = static_cast<int>(i);
                foundValidPath = true;
            }
        }

        // 중복 검사 제거: foundValidPath가 true면 IsLineOnNavMesh 생략
        if (!foundValidPath && !IsLineOnNavMesh(curPos, originalPath[bestNext]))
        {
            Vec3 safe = GetNearestPointOnNavMesh(originalPath[bestNext]);
            outPath.push_back(safe);
        }
        else
        {
            outPath.push_back(originalPath[bestNext]);
        }
        
        cur = bestNext;
        curPos = outPath.back();
    }

    // 마지막 목적지 추가
    outPath.push_back(originalPath.back());
}

void NavMesh::SmoothPathOri(const vector<Vec3>& originalPath, vector<Vec3>& outPath)
{
    outPath.clear();
    if (originalPath.size() <= 2)
    {
        outPath = originalPath; // 작은 경우에만 복사
        return;
    }

    // 예상 크기로 메모리 미리 할당 (원본보다 작을 것으로 예상)
    outPath.reserve(originalPath.size());
    outPath.push_back(originalPath[0]);

    const float minDist = 3.0f, maxDist = 10.0f;
    size_t cur = 0;

    while (cur + 1 < originalPath.size())
    {
        Vec3 curPos = outPath.back();
        vector<size_t> candidates;
        candidates.reserve(10); // 일반적인 후보 개수 예상

        // 시야 확보된 모든 지점 수집
        for (size_t i = cur + 1; i < originalPath.size(); ++i)
        {
            if (HasLineOfSight(curPos, originalPath[i]))
            {
                float d = GetDistance(curPos, originalPath[i]);
                if (d >= minDist && d <= maxDist)
                    candidates.push_back(i);
            }
            else break;
        }

        // 후보 중 가장 진행도가 높은(가장 뒤) 지점 선택
        size_t next = (candidates.empty() ? cur + 1 : candidates.back());

        // NavMesh 외 추돌 검사
        if (!IsLineOnNavMesh(curPos, originalPath[next]))
        {
            Vec3 safe = GetNearestPointOnNavMesh(originalPath[next]);
            outPath.push_back(safe);
        }
        else
        {
            outPath.push_back(originalPath[next]);
        }
        cur = next;
    }

    // 마지막 목적지 추가
    outPath.push_back(originalPath.back());
}

// Private 유틸리티 함수들
void NavMesh::BuildTriangleConnections()
{
    for (size_t i = 0; i < m_triangles.size(); i++)
    {
        m_triangles[i].index = static_cast<int>(i);
        m_triangles[i].neighbors.clear();

        for (size_t j = 0; j < m_triangles.size(); j++)
        {
            if (i != j && AreTrianglesAdjacent(m_triangles[i], m_triangles[j]))
            {
                m_triangles[i].neighbors.push_back(static_cast<int>(j));
            }
        }
    }
}

void NavMesh::InitializeSpatialGrid()
{
    m_spatialGrid.grid.clear();

    for (size_t i = 0; i < m_triangles.size(); ++i) {
        m_spatialGrid.AddTriangle(static_cast<int>(i), m_triangles[i]);
    }
}

bool NavMesh::AreTrianglesAdjacent(const NavMeshTriangle& tri1, const NavMeshTriangle& tri2)
{
    const float EPSILON = 0.01f;
    int sharedVertices = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (GetDistance(tri1.vertices[i], tri2.vertices[j]) < EPSILON)
            {
                sharedVertices++;
                break;
            }
        }
    }

    return sharedVertices >= 2;
}

int NavMesh::FindTriangleContaining(const Vec3& point)
{
    if (m_lastFoundTriangle != -1 && IsPointInTriangle(point, m_triangles[m_lastFoundTriangle])) {
        return m_lastFoundTriangle;
    }

    if (m_lastFoundTriangle != -1) {
        for (int neighbor : m_triangles[m_lastFoundTriangle].neighbors) {
            if (IsPointInTriangle(point, m_triangles[neighbor])) {
                m_lastFoundTriangle = neighbor;
                return neighbor;
            }
        }
    }

    uint64 key = m_spatialGrid.GetKey(point);
    auto it = m_spatialGrid.grid.find(key);

    if (it != m_spatialGrid.grid.end()) {
        for (int idx : it->second) {
            if (IsPointInTriangle(point, m_triangles[idx]))
            {
                return idx;
            }
        }
    }


    //전수 조사 방식. 
    /*for (size_t i = 0; i < m_triangles.size(); i++)
    {
        if (IsPointInTriangle(point, m_triangles[i]))
        {
            return static_cast<int>(i);
        }
    }*/
    return -1;
}

namespace
{
    // 삼각형 평면 위에서 (x, z) 지점의 높이. 수평 위치는 그대로 두고 높이만 구한다.
    float HeightOnTriangle(const NavMeshTriangle& tri, float x, float z, float fallbackY)
    {
        const Vec3& n = tri.normal;
        if (fabsf(n.y) < 1e-4f)
            return fallbackY;   // 수직에 가까운 면
        const Vec3& v0 = tri.vertices[0];
        return v0.y - (n.x * (x - v0.x) + n.z * (z - v0.z)) / n.y;
    }
}

Vec3 NavMesh::ProjectPointOnTriangle(const Vec3& point, const NavMeshTriangle& triangle)
{
    Vec3 v0 = triangle.vertices[0];
    // 수직으로 평면에 맞춘다 (기존: 법선 방향 투영 → 경사면에서 수평 위치까지 밀려남)
    Vec3 planePoint = point;
    planePoint.y = HeightOnTriangle(triangle, point.x, point.z, point.y);

    if (IsPointInTriangle(planePoint, triangle))
    {
        return planePoint;
    }

    // 삼각형 외부에 있으면 가장 가까운 정점 반환
    Vec3 closestPoint = triangle.vertices[0];
    float minDist = GetDistance(point, triangle.vertices[0]);

    for (int i = 1; i < 3; i++)
    {
        float dist = GetDistance(point, triangle.vertices[i]);
        if (dist < minDist)
        {
            minDist = dist;
            closestPoint = triangle.vertices[i];
        }
    }

    return closestPoint;
}

// 높이(y)를 무시하고 XZ 평면에서 판정한다.
// (기존: 3D 내적이라 경사면에서는 '법선 방향' 기준이 되어, 수직으로 위/아래에 있는 점도 밖으로 판정될 수 있었음)
bool NavMesh::IsPointInTriangle(const Vec3& point, const NavMeshTriangle& triangle)
{
    const Vec3& a = triangle.vertices[0];
    const Vec3& b = triangle.vertices[1];
    const Vec3& c = triangle.vertices[2];

    const float v0x = c.x - a.x, v0z = c.z - a.z;
    const float v1x = b.x - a.x, v1z = b.z - a.z;
    const float v2x = point.x - a.x, v2z = point.z - a.z;

    const float dot00 = v0x * v0x + v0z * v0z;
    const float dot01 = v0x * v1x + v0z * v1z;
    const float dot02 = v0x * v2x + v0z * v2z;
    const float dot11 = v1x * v1x + v1z * v1z;
    const float dot12 = v1x * v2x + v1z * v2z;

    const float denom = dot00 * dot11 - dot01 * dot01;
    if (fabsf(denom) < 1e-12f)
        return false;   // XZ 로 보면 면적이 0 인 (수직) 삼각형

    const float invDenom = 1.0f / denom;
    const float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    const float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    const float eps = 1e-5f;   // 변/꼭짓점 위의 점이 부동소수 오차로 양쪽 삼각형 모두에서 빠지지 않게
    return (u >= -eps) && (v >= -eps) && (u + v <= 1.0f + eps);
}

bool NavMesh::HasLineOfSight(const Vec3& start, const Vec3& end)
{
    return GetDistance(start, end) < 0.1f || IsLineOnNavMesh(start, end);
}

bool NavMesh::IsLineOnNavMesh(const Vec3& start, const Vec3& end, float stepSize)
{
    Vec3 direction = end - start;
    float distance = direction.Length();
    if (distance < 0.05f) return true;

    direction.Normalize();
    
    int maxSamples = 16;
    float adaptiveStepSize = max(stepSize, distance / maxSamples);
    int steps = static_cast<int>(distance / stepSize) + 1;
    for (int i = 1; i < steps; i++)
    {
        float t = static_cast<float>(i) / steps;
        Vec3 checkPoint = start + (end - start) * t;
        if (!IsOnNavMesh(checkPoint, 1.0f))
        {
            return false;
        }
    }

    return true;
}

float NavMesh::GetDistance(const Vec3& a, const Vec3& b)
{
    return Vec3::Distance(a, b);
}

int NavMesh::GetTotalNeighbors() const
{
    int total = 0;
    for (const auto& tri : m_triangles)
    {
        total += static_cast<int>(tri.neighbors.size());
    }
    return total;
}

void NavMesh::DebugPrintTriangles()
{
    cout << "=== NavMesh Debug Info ===" << endl;
    cout << "Total triangles: " << m_triangles.size() << endl;
    if (!m_triangles.empty())
    {
        cout << "Average neighbors per triangle: "
            << static_cast<float>(GetTotalNeighbors()) / m_triangles.size() << endl;
    }
}

// =============================================================================
// 경로 진단
// =============================================================================

// 전체 삼각형을 경계 오차 허용으로 검사 (느림, FindTriangleContaining 실패 원인 분석용)
int NavMesh::FindTriangleBruteForce(const Vec3& worldPos, float epsilon) const
{
    for (size_t i = 0; i < m_triangles.size(); ++i)
    {
        const NavMeshTriangle& tri = m_triangles[i];
        const float v0x = tri.vertices[2].x - tri.vertices[0].x, v0z = tri.vertices[2].z - tri.vertices[0].z;
        const float v1x = tri.vertices[1].x - tri.vertices[0].x, v1z = tri.vertices[1].z - tri.vertices[0].z;
        const float v2x = worldPos.x - tri.vertices[0].x, v2z = worldPos.z - tri.vertices[0].z;

        const float dot00 = v0x * v0x + v0z * v0z;
        const float dot01 = v0x * v1x + v0z * v1z;
        const float dot02 = v0x * v2x + v0z * v2z;
        const float dot11 = v1x * v1x + v1z * v1z;
        const float dot12 = v1x * v2x + v1z * v2z;

        const float denom = dot00 * dot11 - dot01 * dot01;
        if (fabsf(denom) < 1e-12f)
            continue;

        const float invDenom = 1.0f / denom;
        const float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        const float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        if (u >= -epsilon && v >= -epsilon && (u + v) <= 1.0f + epsilon)
            return static_cast<int>(i);
    }
    return -1;
}

const char* NavMesh::BranchName(NavPathBranch branch)
{
    switch (branch)
    {
    case NavPathBranch::SameTriangle:     return "SameTriangle";
    case NavPathBranch::TriangleNotFound: return "TriangleNotFound(straight fallback)";
    case NavPathBranch::AStarFailed:      return "AStarFailed(straight fallback)";
    case NavPathBranch::AStarSmoothed:    return "AStar+Smooth";
    default:                              return "None";
    }
}

// =============================================================================
// 통로(Corridor) 기반 스무딩
//  - A*가 고른 삼각형 목록(통로)을 표시해 두고
//  - "현재 점 → 뒤쪽 점" 선분이 통로 삼각형만 지나는지 삼각형을 따라 걸으며 확인
//  - 가장 먼 '통과' 점으로 건너뛴다 (오차 허용 · 샘플링 없음)
// =============================================================================
namespace
{
    constexpr float CORRIDOR_EPS = 1e-5f;

    // XZ 평면 2D 외적
    float Cross2D(float ax, float az, float bx, float bz)
    {
        return ax * bz - az * bx;
    }

    bool SameVertexPos(const Vec3& a, const Vec3& b)
    {
        return Vec3::Distance(a, b) < 0.01f; // AreTrianglesAdjacent 와 같은 기준
    }
}

void NavMesh::BuildEdgeNeighbors()
{
    for (NavMeshTriangle& tri : m_triangles)
    {
        for (int e = 0; e < 3; ++e)
        {
            tri.edgeNeighbor[e] = -1;
            const Vec3& a = tri.vertices[e];
            const Vec3& b = tri.vertices[(e + 1) % 3];

            for (int n : tri.neighbors)
            {
                const NavMeshTriangle& other = m_triangles[n];
                bool hasA = false, hasB = false;
                for (int k = 0; k < 3; ++k)
                {
                    hasA = hasA || SameVertexPos(a, other.vertices[k]);
                    hasB = hasB || SameVertexPos(b, other.vertices[k]);
                }
                if (hasA && hasB)
                {
                    tri.edgeNeighbor[e] = n;
                    break;
                }
            }
        }
    }

    // 경계 꼭짓점 표시: 경계 변(-1)의 양 끝 좌표를 0.01 단위로 키를 만들어 모아 두고,
    // 모든 삼각형 꼭짓점을 그 키로 확인한다 (정점이 병합돼 있지 않아서 좌표로 비교)
    auto vertexKey = [](const Vec3& v) -> uint64 {
        const int32 x = static_cast<int32>(floorf(v.x * 100.f + 0.5f));
        const int32 z = static_cast<int32>(floorf(v.z * 100.f + 0.5f));
        return (static_cast<uint64>(static_cast<uint32>(x)) << 32) | static_cast<uint64>(static_cast<uint32>(z));
    };
    unordered_set<uint64> boundaryKeys;
    for (const NavMeshTriangle& tri : m_triangles)
    {
        for (int e = 0; e < 3; ++e)
        {
            if (tri.edgeNeighbor[e] == -1)
            {
                boundaryKeys.insert(vertexKey(tri.vertices[e]));
                boundaryKeys.insert(vertexKey(tri.vertices[(e + 1) % 3]));
            }
        }
    }
    for (NavMeshTriangle& tri : m_triangles)
        for (int k = 0; k < 3; ++k)
            tri.boundaryVertex[k] = boundaryKeys.count(vertexKey(tri.vertices[k])) > 0;

    m_corridorMask.assign(m_triangles.size(), 0);
}

bool NavMesh::TraceSegmentInCorridor(const Vec3& from, int fromTri, const Vec3& to, const vector<uint8>& inCorridor, int stopTri, int& outEndTri) const
{
    outEndTri = -1;
    if (fromTri < 0 || fromTri >= static_cast<int>(m_triangles.size()) || !inCorridor[fromTri])
        return false;

    const float dx = to.x - from.x;
    const float dz = to.z - from.z;
    if (dx * dx + dz * dz < 1e-12f)
    {
        outEndTri = fromTri;   // 길이 0
        return true;
    }

    int cur = fromTri;
    int prev = -1;
    float sEnter = 0.f;
    const int maxSteps = static_cast<int>(m_triangles.size());

    for (int step = 0; step < maxSteps; ++step)
    {
        if (cur == stopTri)
        {
            outEndTri = cur;   // 도착점이 들어 있는 삼각형까지 통로 안에서 이어짐
            return true;
        }

        // 선분이 현재 삼각형에서 빠져나가는 변 찾기 (들어온 변 제외, 가장 멀리서 만나는 변)
        const NavMeshTriangle& tri = m_triangles[cur];
        int exitEdge = -1;
        float exitS = -1.f;
        for (int e = 0; e < 3; ++e)
        {
            if (prev != -1 && tri.edgeNeighbor[e] == prev)
                continue;

            const Vec3& a = tri.vertices[e];
            const Vec3& b = tri.vertices[(e + 1) % 3];
            const float ex = b.x - a.x, ez = b.z - a.z;

            const float denom = Cross2D(dx, dz, ex, ez);
            if (fabsf(denom) < 1e-12f)
                continue; // 평행

            const float wx = a.x - from.x, wz = a.z - from.z;
            const float s = Cross2D(wx, wz, ex, ez) / denom;   // 선분 위 위치 (0 = from, 1 = to)
            const float u = Cross2D(wx, wz, dx, dz) / denom;   // 변 위 위치 (0 = a, 1 = b)

            if (u < -CORRIDOR_EPS || u > 1.f + CORRIDOR_EPS)
                continue;
            if (s < sEnter - CORRIDOR_EPS)
                continue;
            if (s > exitS)
            {
                exitS = s;
                exitEdge = e;
            }
        }

        if (exitEdge == -1)
            return false;          // 수치 문제 → 보수적으로 실패 처리
        if (exitS >= 1.f - CORRIDOR_EPS)
        {
            outEndTri = cur;       // 도착점이 현재(통로) 삼각형 안에서 끝남
            return true;
        }

        const int next = tri.edgeNeighbor[exitEdge];
        if (next == -1)
            return false;          // 경계 변 = 벽 → NavMesh 밖으로 나감
        if (!inCorridor[next])
            return false;          // NavMesh 위지만 A* 통로 밖으로 나감

        prev = cur;
        cur = next;
        sEnter = exitS;
    }
    return false;
}

bool NavMesh::IsSegmentOnNavMesh(const Vec3& from, const Vec3& to)
{
    const int fromTri = FindTriangleContaining(from);
    if (fromTri < 0)
        return false;
    if (m_allMask.size() != m_triangles.size())
        m_allMask.assign(m_triangles.size(), 1);
    int endTri = -1;
    return TraceSegmentInCorridor(from, fromTri, to, m_allMask, -1, endTri);
}

bool NavMesh::IsSegmentInCorridor(const Vec3& from, int fromTri, const Vec3& to, int toTri, const vector<uint8>& inCorridor) const
{
    int endTri = -1;
    return TraceSegmentInCorridor(from, fromTri, to, inCorridor, toTri, endTri);
}

// 통로 표시(m_corridorMask)는 호출하는 쪽(FindPath)에서 한다
void NavMesh::SmoothPathCorridor(const vector<Vec3>& rawPath, const vector<int>& trianglePath, vector<Vec3>& outPath, vector<int>& outTris)
{
    outPath.clear();
    outTris.clear();
    const size_t n = rawPath.size();
    if (n <= 2 || trianglePath.size() != n)
    {
        outPath = rawPath;
        outTris = (trianglePath.size() == n) ? trianglePath : vector<int>(n, -1);
        return;
    }

    outPath.reserve(n);
    outTris.reserve(n);
    outPath.push_back(rawPath[0]);
    outTris.push_back(trianglePath[0]);

    size_t cur = 0;
    while (cur + 1 < n)
    {
        // 가장 먼 점부터 확인 → 처음 통과하는 점이 '가장 먼 통과 점'
        size_t next = cur + 1;
        for (size_t i = n - 1; i > cur + 1; --i)
        {
            if (IsSegmentInCorridor(rawPath[cur], trianglePath[cur], rawPath[i], trianglePath[i], m_corridorMask))
            {
                next = i;
                break;
            }
        }
        // 건너뛸 점이 없어 바로 옆 삼각형 중점으로 갈 때도 확인한다.
        // 두 중점을 바로 이으면 오목한 벽 코너 바깥을 스칠 수 있음 (가늘고 긴 삼각형)
        // → 두 삼각형의 공유 변 중점을 거쳐 간다 (삼각형은 볼록이라 두 선분 모두 NavMesh 안)
        if (next == cur + 1 &&
            !IsSegmentInCorridor(rawPath[cur], trianglePath[cur], rawPath[next], trianglePath[next], m_corridorMask))
        {
            const NavMeshTriangle& tri = m_triangles[trianglePath[cur]];
            for (int e = 0; e < 3; ++e)
            {
                if (tri.edgeNeighbor[e] == trianglePath[next])
                {
                    outPath.push_back((tri.vertices[e] + tri.vertices[(e + 1) % 3]) * 0.5f);
                    outTris.push_back(trianglePath[next]);
                    break;
                }
            }
        }
        outPath.push_back(rawPath[next]);
        outTris.push_back(trianglePath[next]);
        cur = next;
    }
}

// =============================================================================
// 가이드 분할 스무딩 (CorridorSplit)
//  Funnel 처럼 Portal / 깔때기를 쓰지 않는다. 중점 경로(A* 원시 경로)를 '가이드'로 쓴다.
//  1) 출발 → 도착 직선이 ① A* 통로 안 ② 벽 꼭짓점에서 r 이상 이면 그대로 쓴다
//  2) 막히면: 그 직선과 가이드(중점 경로) 사이로 튀어나온 벽 꼭짓점 중 가장 깊은 V 를 찾고,
//     V 에서 가장 가까운 중점 방향으로 r 떨어진 점 P 를 꺾는 점으로 넣는다 → 출발→P, P→도착 을 같은 방식으로 (재귀)
//  3) 마무리: 꺾는 점마다 앞뒤 선분이 원(V, r)에 접하도록 위치 조정 + 필요 없어진 점 제거 (변화 없을 때까지)
//  r = m_agentRadius. 출발/도착이 원래 벽에 붙어 있는 구간은 '중점 경로보다 나빠지지 않게'까지만 보장
// =============================================================================
namespace
{
    struct PtXZ { float x, z; };

    float LenXZ2(float x0, float z0, float x1, float z1) { return sqrtf((x1 - x0) * (x1 - x0) + (z1 - z0) * (z1 - z0)); }

    float SegPointDistXZ(const PtXZ& v, const Vec3& a, const Vec3& b)
    {
        const float abx = b.x - a.x, abz = b.z - a.z;
        const float len2 = abx * abx + abz * abz;
        float tt = (len2 < 1e-12f) ? 0.f : ((v.x - a.x) * abx + (v.z - a.z) * abz) / len2;
        tt = max(0.f, min(1.f, tt));
        return LenXZ2(a.x + abx * tt, a.z + abz * tt, v.x, v.z);
    }

    // 선분 a-b 에서 벽 꼭짓점까지 최소 거리. skipEnds = 선분 끝과 겹치는 꼭짓점은 제외
    float ClearanceXZ(const Vec3& a, const Vec3& b, const vector<PtXZ>& verts, bool skipEnds)
    {
        float best = 1e9f;
        for (const PtXZ& v : verts)
        {
            if (skipEnds && (LenXZ2(v.x, v.z, a.x, a.z) <= 1e-3f || LenXZ2(v.x, v.z, b.x, b.z) <= 1e-3f))
                continue;
            best = min(best, SegPointDistXZ(v, a, b));
        }
        return best;
    }

    bool PointInPolygonXZ(const PtXZ& p, const vector<PtXZ>& poly)
    {
        bool inside = false;
        const size_t n = poly.size();
        for (size_t k = 0; k < n; ++k)
        {
            const PtXZ& p1 = poly[k];
            const PtXZ& p2 = poly[(k + 1) % n];
            if ((p1.z > p.z) != (p2.z > p.z))
            {
                const float xi = p1.x + (p.z - p1.z) * (p2.x - p1.x) / (p2.z - p1.z);
                if (xi > p.x) inside = !inside;
            }
        }
        return inside;
    }

    // (a, cur, b) 삼각형 안 벽 꼭짓점 중 선분 a-b 에서 가장 먼 것
    bool CornerVertexXZ(const vector<PtXZ>& verts, const Vec3& a, const Vec3& cur, const Vec3& b, PtXZ& outV)
    {
        const float acx = b.x - a.x, acz = b.z - a.z;
        const float base = sqrtf(acx * acx + acz * acz);
        const float o = Cross2D(cur.x - a.x, cur.z - a.z, acx, acz);
        if (base < 1e-6f || fabsf(o) < 1e-6f * base)
            return false;
        const float sg = (o > 0.f) ? 1.f : -1.f;
        float bestD = 0.f;
        bool found = false;
        for (const PtXZ& v : verts)
        {
            if (Cross2D(cur.x - a.x, cur.z - a.z, v.x - a.x, v.z - a.z) * sg < -1e-6f) continue;
            if (Cross2D(b.x - cur.x, b.z - cur.z, v.x - cur.x, v.z - cur.z) * sg < -1e-6f) continue;
            if (Cross2D(a.x - b.x, a.z - b.z, v.x - b.x, v.z - b.z) * sg < -1e-6f) continue;
            const float d = fabsf(Cross2D(acx, acz, v.x - a.x, v.z - a.z)) / base;
            if (d > bestD) { bestD = d; outV = v; found = true; }
        }
        return found;
    }

    int TangentPointsXZ(const Vec3& p, const PtXZ& v, float r, PtXZ out[2])
    {
        const float dx = p.x - v.x, dz = p.z - v.z;
        const float d = sqrtf(dx * dx + dz * dz);
        if (d <= r * 1.0001f)
            return 0;
        const float th = atan2f(dz, dx), al = acosf(r / d);
        out[0] = { v.x + r * cosf(th + al), v.z + r * sinf(th + al) };
        out[1] = { v.x + r * cosf(th - al), v.z + r * sinf(th - al) };
        return 2;
    }

    bool LineIntersectXZ(const PtXZ& p1, const PtXZ& p2, const PtXZ& p3, const PtXZ& p4, PtXZ& out)
    {
        const float d = (p1.x - p2.x) * (p3.z - p4.z) - (p1.z - p2.z) * (p3.x - p4.x);
        if (fabsf(d) < 1e-12f)
            return false;
        const float tt = ((p1.x - p3.x) * (p3.z - p4.z) - (p1.z - p3.z) * (p3.x - p4.x)) / d;
        out = { p1.x + tt * (p2.x - p1.x), p1.z + tt * (p2.z - p1.z) };
        return true;
    }

    // a → Q → b 두 선분이 모두 원(v, r)에 접하는 꺾는 점 Q (cur 쪽, 가장 짧은 것)
    bool WrapPointXZ(const Vec3& a, const Vec3& cur, const Vec3& b, const PtXZ& v, float r, PtXZ& outQ)
    {
        const float sgc = Cross2D(b.x - a.x, b.z - a.z, cur.x - a.x, cur.z - a.z);
        PtXZ ta[2], tb[2];
        const int na = TangentPointsXZ(a, v, r, ta), nb = TangentPointsXZ(b, v, r, tb);
        float bestLen = FLT_MAX;
        bool found = false;
        for (int i = 0; i < na; ++i)
            for (int j = 0; j < nb; ++j)
            {
                PtXZ q;
                if (!LineIntersectXZ({ a.x, a.z }, ta[i], { b.x, b.z }, tb[j], q)) continue;
                if (Cross2D(b.x - a.x, b.z - a.z, q.x - a.x, q.z - a.z) * sgc <= 0.f) continue;   // cur 쪽
                const float s1 = Cross2D(q.x - a.x, q.z - a.z, v.x - a.x, v.z - a.z);
                const float s2 = Cross2D(b.x - q.x, b.z - q.z, v.x - q.x, v.z - q.z);
                const float s3 = Cross2D(a.x - b.x, a.z - b.z, v.x - b.x, v.z - b.z);
                if (!((s1 >= 0.f && s2 >= 0.f && s3 >= 0.f) || (s1 <= 0.f && s2 <= 0.f && s3 <= 0.f))) continue;   // V 를 감싸는 쪽
                const float len = LenXZ2(a.x, a.z, q.x, q.z) + LenXZ2(q.x, q.z, b.x, b.z);
                if (len < bestLen) { bestLen = len; outQ = q; found = true; }
            }
        return found;
    }

    float SegPointDist2XZ(const PtXZ& v, const Vec3& a, const Vec3& b)
    {
        const float abx = b.x - a.x, abz = b.z - a.z;
        const float len2 = abx * abx + abz * abz;
        float tt = (len2 < 1e-12f) ? 0.f : ((v.x - a.x) * abx + (v.z - a.z) * abz) / len2;
        tt = max(0.f, min(1.f, tt));
        const float dx = a.x + abx * tt - v.x, dz = a.z + abz * tt - v.z;
        return dx * dx + dz * dz;
    }

    class CorridorSplitter
    {
    public:
        CorridorSplitter(const NavMesh& nav, const vector<uint8>& mask, const vector<Vec3>& raw, const vector<int>& tp, float r, bool fan)
            : m_nav(nav), m_mask(mask), m_raw(raw), m_tp(tp), m_r(r), m_fan(fan)
        {
            // [최적화] 통로 벽 꼭짓점 중복 제거 (정점이 병합돼 있지 않아 같은 꼭짓점이 삼각형마다 따로 들어옴)
            unordered_set<uint64> seen;
            for (int ti : tp)
            {
                const NavMeshTriangle& tri = nav.GetTriangle(ti);
                for (int k = 0; k < 3; ++k)
                {
                    if (!tri.boundaryVertex[k]) continue;
                    const Vec3& v = tri.vertices[k];
                    const int32 kx = static_cast<int32>(floorf(v.x * 1000.f + 0.5f));
                    const int32 kz = static_cast<int32>(floorf(v.z * 1000.f + 0.5f));
                    const uint64 key = (static_cast<uint64>(static_cast<uint32>(kx)) << 32) | static_cast<uint32>(kz);
                    if (seen.insert(key).second)
                        m_verts.push_back({ v.x, v.z });
                }
            }
            const int n = static_cast<int>(raw.size());
            m_guideClear.resize(max(0, n - 1));
            for (int k = 0; k + 1 < n; ++k)
                m_guideClear[k] = Clear(raw[k], raw[k + 1], false);
        }

        void Run(vector<Vec3>& outPath, vector<int>& outTris)
        {
            const int n = static_cast<int>(m_raw.size());
            m_pts.clear(); m_tris.clear();
            m_pts.reserve(n * 2); m_tris.reserve(n * 2);
            m_pts.push_back(m_raw[0]); m_tris.push_back(m_tp[0]);
            Rec(m_raw[0], m_tp[0], 0, m_raw[n - 1], m_tp[n - 1], n - 1, 0, nullptr, nullptr);

            if (m_fan)
            {
                // 부채꼴 점은 처음부터 제자리 → 접선 감싸기 반복 없이 정리 한 번 (여유 기준 0.5r: 부채꼴 현이 r 보다 가까워지는 것 허용)
                SkipPass(0.5f);
            }
            else
            {
                for (int it = 0; it < 20; ++it)
                {
                    const int changed = WrapPass();
                    SkipPass(1.f);
                    if (changed == 0) break;
                }
            }
            outPath = m_pts;
            outTris = m_tris;
        }

    private:
        bool Seg(const Vec3& a, int ta, const Vec3& b, int tb) const { return m_nav.IsSegmentInCorridor(a, ta, b, tb, m_mask); }
        int SegEnd(const Vec3& a, int ta, const Vec3& b) const
        {
            int endTri = -1;
            return m_nav.TraceSegmentInCorridor(a, ta, b, m_mask, -1, endTri) ? endTri : -1;
        }

        // [최적화] 선분 a-b 에서 벽 꼭짓점까지 최소 거리, r 에서 자름.
        //  호출하는 곳은 모두 'r 이하인 값'과만 비교하므로 r 보다 먼 꼭짓점은 볼 필요가 없다
        //  → 선분 경계 상자(+r) 밖 꼭짓점은 바로 건너뛴다. (자르기 전과 판정 결과가 같다)
        static bool SameXZ(const PtXZ& p, const PtXZ* q) { return q && fabsf(p.x - q->x) < 1e-4f && fabsf(p.z - q->z) < 1e-4f; }

        float Clear(const Vec3& a, const Vec3& b, bool skipEnds, const PtXZ* exA = nullptr, const PtXZ* exB = nullptr) const
        {
            const float cap = m_r;
            const float minX = min(a.x, b.x) - cap, maxX = max(a.x, b.x) + cap;
            const float minZ = min(a.z, b.z) - cap, maxZ = max(a.z, b.z) + cap;
            float best2 = cap * cap;
            const PtXZ* v = m_verts.data();
            for (size_t k = 0, n = m_verts.size(); k < n; ++k)
            {
                const PtXZ& p = v[k];
                if (p.x < minX || p.x > maxX || p.z < minZ || p.z > maxZ) continue;
                if (skipEnds)
                {
                    const float ax = p.x - a.x, az = p.z - a.z, bx = p.x - b.x, bz = p.z - b.z;
                    if (ax * ax + az * az <= 1e-6f || bx * bx + bz * bz <= 1e-6f) continue;
                }
                if (SameXZ(p, exA) || SameXZ(p, exB)) continue;   // 부채꼴 점의 기준 꼭짓점 (그 점은 이미 V 에서 r)
                best2 = min(best2, SegPointDist2XZ(p, a, b));
            }
            return sqrtf(best2);
        }
        float GuideClear(int ia, int ib) const
        {
            if (ib <= ia) return m_r;
            float c = FLT_MAX;
            for (int k = ia; k < ib; ++k) c = min(c, m_guideClear[k]);
            return c;
        }
        float Height(int tri, const Vec3& p, float fallback) const { return HeightOnTriangle(m_nav.GetTriangle(tri), p.x, p.z, fallback); }

        // 이웃 삼각형 점 두 개를 잇는다. 막히면 공유 변 중점 경유 (Corridor 와 같은 보정)
        void AddAdjacent(Vec3 p0, int t0, Vec3 p1, int t1)   // 값 복사: m_pts.back() 을 넘겨도 안전
        {
            if (!Seg(p0, t0, p1, t1))
            {
                const NavMeshTriangle& tri = m_nav.GetTriangle(t0);
                for (int e = 0; e < 3; ++e)
                    if (tri.edgeNeighbor[e] == t1)
                    {
                        m_pts.push_back((tri.vertices[e] + tri.vertices[(e + 1) % 3]) * 0.5f);
                        m_tris.push_back(t1);
                        break;
                    }
            }
            m_pts.push_back(p1);
            m_tris.push_back(t1);
        }

        void FollowGuide(int ia, const Vec3& b, int tb, int ib)
        {
            for (int k = ia + 1; k <= ib; ++k)
                AddAdjacent(m_pts.back(), m_tris.back(), (k < ib) ? m_raw[k] : b, (k < ib) ? m_tp[k] : tb);
        }

        void Rec(Vec3 a, int ta, int ia, Vec3 b, int tb, int ib, int depth, const PtXZ* exA, const PtXZ* exB)
        {
            const bool ok = Seg(a, ta, b, tb);
            const float need = min(m_r, GuideClear(ia, ib)) - 1e-4f;
            if (ok && Clear(a, b, true, exA, exB) >= need)
            {
                m_pts.push_back(b); m_tris.push_back(tb);
                return;
            }
            if (depth > 40 && ib - ia > 1)
            {
                FollowGuide(ia, b, tb, ib);   // 너무 깊음 → 가이드(중점 경로) 그대로
                return;
            }
            if (ib - ia <= 1)
            {
                if (ok) { m_pts.push_back(b); m_tris.push_back(tb); return; }
                // 막힘 → a → 중점[ia] → (공유 변 중점) → 중점[ib] → b (각 조각은 만들 때 검증됨)
                const Vec3& ra = m_raw[ia];
                const Vec3& rb = m_raw[ib];
                if (LenXZ2(a.x, a.z, ra.x, ra.z) > 1e-4f) { m_pts.push_back(ra); m_tris.push_back(m_tp[ia]); }
                AddAdjacent(ra, m_tp[ia], rb, m_tp[ib]);
                if (LenXZ2(b.x, b.z, rb.x, rb.z) > 1e-4f) { m_pts.push_back(b); m_tris.push_back(tb); }
                return;
            }

            // 직선 a-b 와 가이드 사이 다각형 안에 든 벽 꼭짓점 중 가장 깊은 것
            m_poly.clear();
            m_poly.push_back({ a.x, a.z });
            float pminX = min(a.x, b.x), pmaxX = max(a.x, b.x), pminZ = min(a.z, b.z), pmaxZ = max(a.z, b.z);
            for (int k = ia + 1; k < ib; ++k)
            {
                m_poly.push_back({ m_raw[k].x, m_raw[k].z });
                pminX = min(pminX, m_raw[k].x); pmaxX = max(pmaxX, m_raw[k].x);
                pminZ = min(pminZ, m_raw[k].z); pmaxZ = max(pmaxZ, m_raw[k].z);
            }
            m_poly.push_back({ b.x, b.z });

            const float abx = b.x - a.x, abz = b.z - a.z;
            const float abLen = max(1e-6f, sqrtf(abx * abx + abz * abz));
            bool hasV = false, inPoly = false;
            PtXZ V{};
            float best = 0.f;
            for (const PtXZ& v : m_verts)
            {
                if (v.x < pminX || v.x > pmaxX || v.z < pminZ || v.z > pmaxZ) continue;   // [최적화] 다각형 경계 상자 밖
                if (LenXZ2(v.x, v.z, a.x, a.z) <= 1e-3f || LenXZ2(v.x, v.z, b.x, b.z) <= 1e-3f) continue;
                if (SameXZ(v, exA) || SameXZ(v, exB)) continue;
                if (!PointInPolygonXZ(v, m_poly)) continue;
                const float d = fabsf(Cross2D(abx, abz, v.x - a.x, v.z - a.z)) / abLen;
                if (!inPoly || d > best) { best = d; V = v; inPoly = true; hasV = true; }
            }
            if (!inPoly)
            {
                best = FLT_MAX;
                for (const PtXZ& v : m_verts)
                {
                    if (LenXZ2(v.x, v.z, a.x, a.z) <= 1e-3f || LenXZ2(v.x, v.z, b.x, b.z) <= 1e-3f) continue;
                    if (SameXZ(v, exA) || SameXZ(v, exB)) continue;
                    const float d = SegPointDist2XZ(v, a, b);
                    if (d < best) { best = d; V = v; hasV = true; }
                }
            }
            if (!hasV)
            {
                FollowGuide(ia, b, tb, ib);
                return;
            }

            // [CorridorFan] V 를 끝점으로 갖는 범위 안 Portal 마다 V + r·(변 방향) = 부채꼴 점
            //  연속한 두 부채꼴 점은 같은 삼각형의 두 변 위 → 잇는 선분은 판정 없이 통로 안
            if (m_fan)
            {
                const PtXZ Vc = V;   // 재귀 중 m_verts 는 바뀌지 않지만, 제외 기준은 값으로 들고 간다
                m_fanTmp.clear();
                for (int kk = ia; kk < ib; ++kk)
                {
                    const NavMeshTriangle& tri = m_nav.GetTriangle(m_tp[kk]);
                    for (int e = 0; e < 3; ++e)
                    {
                        if (tri.edgeNeighbor[e] != m_tp[kk + 1]) continue;
                        const Vec3& e0 = tri.vertices[e];
                        const Vec3& e1 = tri.vertices[(e + 1) % 3];
                        const Vec3* v3 = nullptr; const Vec3* o3 = nullptr;
                        if (fabsf(e0.x - Vc.x) < 1e-3f && fabsf(e0.z - Vc.z) < 1e-3f) { v3 = &e0; o3 = &e1; }
                        else if (fabsf(e1.x - Vc.x) < 1e-3f && fabsf(e1.z - Vc.z) < 1e-3f) { v3 = &e1; o3 = &e0; }
                        if (v3)
                        {
                            const Vec3 d3 = *o3 - *v3;
                            const float ld = sqrtf(d3.x * d3.x + d3.z * d3.z);
                            if (ld > 1e-6f)
                                m_fanTmp.push_back({ *v3 + d3 * (min(m_r, 0.5f * ld) / ld), kk });
                        }
                        break;
                    }
                }
                if (!m_fanTmp.empty())
                {
                    const vector<FanPt> fan = m_fanTmp;   // 재귀가 m_fanTmp 를 다시 쓰므로 복사
                    const FanPt& A = fan.front();
                    const FanPt& B = fan.back();
                    Rec(a, ta, ia, A.p, m_tp[A.k], A.k, depth + 1, exA, &Vc);
                    for (const FanPt& f : fan)
                    {
                        if (LenXZ2(m_pts.back().x, m_pts.back().z, f.p.x, f.p.z) <= 1e-5f) continue;
                        m_pts.push_back(f.p);
                        m_tris.push_back(m_tp[f.k + 1]);
                    }
                    Rec(B.p, m_tp[B.k + 1], B.k + 1, b, tb, ib, depth + 1, &Vc, exB);
                    return;
                }
            }

            // V 에 가장 가까운 중점 → 그 방향으로 V 에서 r 떨어진 점 = 꺾는 점
            int k = ia + 1;
            float kd = FLT_MAX;
            for (int kk = ia + 1; kk < ib; ++kk)
            {
                const float d = LenXZ2(m_raw[kk].x, m_raw[kk].z, V.x, V.z);
                if (d < kd) { kd = d; k = kk; }
            }
            const Vec3& c = m_raw[k];
            Vec3 p = c;
            const float dx = c.x - V.x, dz = c.z - V.z;
            const float d = sqrtf(dx * dx + dz * dz);
            if (d > m_r)
            {
                p.x = V.x + dx / d * m_r;
                p.z = V.z + dz / d * m_r;
            }
            int pt = SegEnd(c, m_tp[k], p);
            if (pt < 0) { p = c; pt = m_tp[k]; }
            else p.y = Height(pt, p, c.y);

            Rec(a, ta, ia, p, pt, k, depth + 1, exA, nullptr);
            Rec(p, pt, k, b, tb, ib, depth + 1, nullptr, exB);
        }

        // 꺾는 점마다 앞뒤 선분이 원(V, r)에 접하도록 옮긴다 (실패하면 1/2, 1/4, 1/8 만 옮김)
        int WrapPass()
        {
            int changed = 0;
            for (size_t i = 1; i + 1 < m_pts.size(); ++i)
            {
                const Vec3 a = m_pts[i - 1], cur = m_pts[i], b = m_pts[i + 1];
                PtXZ v, q2;
                if (!CornerVertexXZ(m_verts, a, cur, b, v)) continue;
                if (!WrapPointXZ(a, cur, b, v, m_r, q2)) continue;
                if (LenXZ2(q2.x, q2.z, cur.x, cur.z) <= 1e-4f) continue;   // [최적화] 이미 접해 있음 (옮겨도 안 바뀜)
                const float need = min(m_r, min(Clear(a, cur, false), Clear(cur, b, false))) - 1e-4f;
                float f = 1.f;
                for (int attempt = 0; attempt < 4; ++attempt, f *= 0.5f)
                {
                    Vec3 q = cur;
                    q.x = cur.x + (q2.x - cur.x) * f;
                    q.z = cur.z + (q2.z - cur.z) * f;
                    if (Clear(a, q, false) < need || Clear(q, b, false) < need) continue;   // [최적화] 싼 판정 먼저
                    const int qt = SegEnd(a, m_tris[i - 1], q);
                    if (qt < 0 || !Seg(q, qt, b, m_tris[i + 1])) continue;
                    if (LenXZ2(q.x, q.z, cur.x, cur.z) > 1e-4f) ++changed;
                    q.y = Height(qt, q, cur.y);
                    m_pts[i] = q;
                    m_tris[i] = qt;
                    break;
                }
            }
            return changed;
        }

        // 먼 점부터 거꾸로: 통로 안 + 여유가 기존 구간들보다 나빠지지 않으면 건너뜀
        void SkipPass(float clearFactor)
        {
            const size_t n = m_pts.size();
            if (n <= 2) return;
            m_segc.resize(n - 1);
            for (size_t k = 0; k + 1 < n; ++k)
                m_segc[k] = Clear(m_pts[k], m_pts[k + 1], false);

            m_tmpPts.clear(); m_tmpTris.clear();
            m_tmpPts.push_back(m_pts[0]); m_tmpTris.push_back(m_tris[0]);
            m_runMin.resize(n);
            size_t cur = 0;
            while (cur + 1 < n)
            {
                // [최적화] runMin[j] = min(segc[cur .. j-1]) 를 한 번에 계산 (전: 후보마다 다시 계산)
                float rm = FLT_MAX;
                for (size_t j = cur + 1; j < n; ++j) { rm = min(rm, m_segc[j - 1]); m_runMin[j] = rm; }

                size_t next = cur + 1;
                for (size_t j = n - 1; j > cur + 1; --j)
                {
                    if (Clear(m_pts[cur], m_pts[j], false) < min(m_r * clearFactor, m_runMin[j]) - 1e-4f)
                        continue;   // [최적화] 싼 여유 판정 먼저, 통로 판정은 그다음
                    if (Seg(m_pts[cur], m_tris[cur], m_pts[j], m_tris[j]))
                    {
                        next = j;
                        break;
                    }
                }
                m_tmpPts.push_back(m_pts[next]); m_tmpTris.push_back(m_tris[next]);
                cur = next;
            }
            m_pts.swap(m_tmpPts);
            m_tris.swap(m_tmpTris);
        }

        const NavMesh& m_nav;
        const vector<uint8>& m_mask;
        const vector<Vec3>& m_raw;
        const vector<int>& m_tp;
        float m_r;
        vector<PtXZ> m_verts;        // 통로 삼각형의 벽 꼭짓점 (XZ, 중복 제거)
        vector<float> m_guideClear;  // 중점 경로 구간별 벽 꼭짓점 여유 (r 에서 자름)
        vector<Vec3> m_pts;
        vector<int> m_tris;
        // 재사용 버퍼
        vector<PtXZ> m_poly;
        vector<float> m_segc, m_runMin;
        vector<Vec3> m_tmpPts;
        vector<int> m_tmpTris;
        bool m_fan = false;
        struct FanPt { Vec3 p; int k; };   // k = Portal 번호 (m_tp[k] ↔ m_tp[k+1])
        vector<FanPt> m_fanTmp;
    };

    float PathLength3D(const vector<Vec3>& path)
    {
        float len = 0.f;
        for (size_t i = 1; i < path.size(); ++i)
            len += Vec3::Distance(path[i - 1], path[i]);
        return len;
    }
}

void NavMesh::SmoothPathCorridorSplit(const vector<Vec3>& rawPath, const vector<int>& trianglePath, vector<Vec3>& outPath, vector<int>& outTris, bool fanPoints)
{
    outPath.clear();
    outTris.clear();
    if (rawPath.size() <= 2 || trianglePath.size() != rawPath.size())
    {
        SmoothPathCorridor(rawPath, trianglePath, outPath, outTris);
        return;
    }
    CorridorSplitter splitter(*this, m_corridorMask, rawPath, trianglePath, m_agentRadius, fanPoints);
    splitter.Run(outPath, outTris);

    // 안전장치: 드물게 쪼갤 꼭짓점을 잘못 골라 Corridor 보다 길어지면 Corridor 결과를 쓴다 (Corridor 는 약 10us)
    vector<Vec3> greedy;
    vector<int> greedyTris;
    SmoothPathCorridor(rawPath, trianglePath, greedy, greedyTris);
    if (PathLength3D(greedy) < PathLength3D(outPath) - 1e-3f)
    {
        outPath.swap(greedy);
        outTris.swap(greedyTris);
    }
}

// =============================================================================
// CorridorFanPortal: CorridorFan 과 같은 분할(가장 깊은 벽 꼭짓점) + 부채꼴 점, 판정만 Portal 기준으로 바꾼 버전
//  통로 = 삼각형 사슬 → 선분 a(tp[ia] 안) → b(tp[ib] 안) 가 통로 안  ⇔  사이 Portal(ia .. ib-1) 을 순서대로 지난다
//  (삼각형은 볼록이라 Portal k 와 k+1 을 지나는 사이 구간은 그 삼각형 안)
//  Portal k 한 번 훑기(Probe)로 세 가지를 같이 구한다
//   - 통로 판정 (삼각형 따라 걷기 대신)      : 모든 k 에서 0 <= u_k <= 1, 선분 위 위치 s_k 가 차례로 증가
//   - 꺾을 꼭짓점 V (점-다각형 판정 대신)    : u_k < 0 이면 p[0], u_k > 1 이면 p[1] 이 선분 밖으로 밀려난 꼭짓점
//   - 벽 여유 (통로 전체 벽 꼭짓점 대신)     : 범위 Portal 끝점 + 양 끝 삼각형 꼭짓점 중 벽 꼭짓점
//  [최적화] 여유 판정의 거리 계산을 교점 계산에 합침
//   교점 계산의 c0 = cross(p0 - a, b - a) 는 |c0| / |b - a| = p0 와 직선 사이 거리 (p1 은 c0 - denom).
//   선분 거리 >= 직선 거리 이므로 직선 거리가 지금까지 최소 여유(r 에서 시작) 이상이면 볼 필요 없다
//   → 정확한 선분 거리는 직선에 r 보다 가까운 꼭짓점만 (결과는 전부 계산할 때와 같다)
//  → Rec 한 번이 O(V·n) 에서 O(ib - ia) 로. 꼭짓점 목록 / 중복 제거 해시 / 안전장치(Corridor 재계산)도 없음
// =============================================================================
namespace
{
    struct PortalSeg
    {
        Vec3 p[2];      // 공유 변 양 끝 (tp[k] 의 변 방향 그대로)
        bool wall[2];   // 벽(경계) 꼭짓점인지
    };

    struct ProbeResult
    {
        bool inside = true;       // Portal ia .. ib-1 을 순서대로 지남
        bool hasV = false;        // 지나지 못한 Portal 의 막는 끝점 (선분 직선에서 가장 깊은 것)
        PtXZ V{};
        float clear = 0.f;        // 벽 꼭짓점까지 최소 거리 (r 에서 자름)
        bool hasNearest = false;  // r 보다 가까운 벽 꼭짓점이 있음
        PtXZ nearest{};           // 그중 가장 가까운 것
    };

    class PortalFanSplitter
    {
    public:
        PortalFanSplitter(const NavMesh& nav, const vector<Vec3>& raw, const vector<int>& tp, float r)
            : m_nav(nav), m_raw(raw), m_tp(tp), m_r(r) {}

        // Portal 배열 + 가이드(중점 경로) 구간 여유. 변 이웃이 비어 있으면 false (호출 쪽에서 기존 Fan 사용)
        bool Build()
        {
            const int n = static_cast<int>(m_tp.size());
            m_portals.resize(max(0, n - 1));
            for (int k = 0; k + 1 < n; ++k)
            {
                const NavMeshTriangle& tri = m_nav.GetTriangle(m_tp[k]);
                int e = -1;
                for (int i = 0; i < 3; ++i)
                    if (tri.edgeNeighbor[i] == m_tp[k + 1]) { e = i; break; }
                if (e < 0)
                    return false;
                PortalSeg& ps = m_portals[k];
                ps.p[0] = tri.vertices[e];
                ps.p[1] = tri.vertices[(e + 1) % 3];
                ps.wall[0] = tri.boundaryVertex[e];
                ps.wall[1] = tri.boundaryVertex[(e + 1) % 3];
            }
            m_guideClear.resize(max(0, n - 1));
            ProbeResult pr;
            for (int k = 0; k + 1 < n; ++k)
            {
                Probe(m_raw[k], k, m_raw[k + 1], k + 1, false, nullptr, nullptr, false, true, pr);
                m_guideClear[k] = pr.clear;
            }
            return true;
        }

        void Run(vector<Vec3>& outPath, vector<int>& outTris)
        {
            const int n = static_cast<int>(m_raw.size());
            m_pts.clear(); m_idx.clear(); m_fan.clear();
            m_pts.reserve(n * 2); m_idx.reserve(n * 2);
            m_pts.push_back(m_raw[0]); m_idx.push_back(0);
            Rec(m_raw[0], 0, m_raw[n - 1], n - 1, 0, nullptr, nullptr);
            SkipPass(0.5f);   // CorridorFan 과 같은 정리 한 번 (여유 기준 0.5r)

            outPath = m_pts;
            outTris.resize(m_idx.size());
            for (size_t i = 0; i < m_idx.size(); ++i)
                outTris[i] = m_tp[m_idx[i]];
        }

    private:
        static bool SameXZ(const Vec3& p, const PtXZ* q, float eps = 1e-4f)
        {
            return q && fabsf(p.x - q->x) < eps && fabsf(p.z - q->z) < eps;
        }
        static bool NearEnd(const Vec3& v, const Vec3& a, const Vec3& b)
        {
            const float ax = v.x - a.x, az = v.z - a.z, bx = v.x - b.x, bz = v.z - b.z;
            return ax * ax + az * az <= 1e-6f || bx * bx + bz * bz <= 1e-6f;
        }

        // 선분 a(ia) → b(ib) 를 Portal 한 번 훑기로 판정
        //  wantV    : 지나지 못한 Portal 에서 막는 꼭짓점 V 를 고른다
        //  wantClear: 벽 꼭짓점 여유 (r 에서 자름). 대상 = Portal ia-1 .. ib 끝점 + tp[ia], tp[ib] 꼭짓점 중 벽
        //  skipEnds : 선분 끝과 겹치는 꼭짓점 제외 / exA, exB: 부채꼴 점의 기준 꼭짓점 제외 (그 점은 이미 V 에서 r)
        void Probe(const Vec3& a, int ia, const Vec3& b, int ib, bool skipEnds, const PtXZ* exA, const PtXZ* exB,
                   bool wantV, bool wantClear, ProbeResult& out) const
        {
            out = ProbeResult();
            float best2 = m_r * m_r;   // 지금까지 최소 여유² (r 이상은 볼 필요 없음)
            auto consider = [&](const Vec3& v)   // 정확한 선분 거리
            {
                if (skipEnds && NearEnd(v, a, b)) return;
                if (SameXZ(v, exA) || SameXZ(v, exB)) return;
                const float d2 = SegPointDist2XZ({ v.x, v.z }, a, b);
                if (d2 < best2) { best2 = d2; out.nearest = { v.x, v.z }; out.hasNearest = true; }
            };

            const float dx = b.x - a.x, dz = b.z - a.z;
            const float len2 = dx * dx + dz * dz;
            const bool degenerate = len2 < 1e-12f;   // 길이 0: 통로 안, 여유는 점 a 기준
            const float len = sqrtf(len2);
            float sPrev = -1e-4f;
            float deepest = -1.f;
            for (int k = ia; k < ib; ++k)
            {
                const PortalSeg& ps = m_portals[k];
                const float ex = ps.p[1].x - ps.p[0].x, ez = ps.p[1].z - ps.p[0].z;
                const float wx = ps.p[0].x - a.x, wz = ps.p[0].z - a.z;
                const float denom = Cross2D(dx, dz, ex, ez);
                const float c0 = Cross2D(wx, wz, dx, dz);   // |c0| / len = p0 와 직선 거리 (= u · denom)
                const float c1 = c0 - denom;                // p1 쪽

                if (wantClear)
                {
                    // 직선 거리² (c²/len²) 가 best2 이상이면 선분 거리도 그 이상 → 건너뜀
                    if (ps.wall[0] && (degenerate || c0 * c0 < best2 * len2)) consider(ps.p[0]);
                    if (ps.wall[1] && (degenerate || c1 * c1 < best2 * len2)) consider(ps.p[1]);
                }
                if (degenerate)
                    continue;

                if (fabsf(denom) < 1e-12f) { out.inside = false; continue; }   // 평행 → 못 지남
                const float s = Cross2D(wx, wz, ex, ez) / denom;   // 선분 위 위치 (0 = a, 1 = b)
                const float u = c0 / denom;                        // Portal 위 위치 (0 = p[0], 1 = p[1])
                if (s < sPrev - 1e-4f || s > 1.f + 1e-4f)
                    out.inside = false;   // 순서가 어긋나거나 선분 밖 (접힌 통로)
                sPrev = max(sPrev, s);

                int side = -1;
                if (u < -CORRIDOR_EPS) side = 0;
                else if (u > 1.f + CORRIDOR_EPS) side = 1;
                if (side < 0)
                    continue;
                out.inside = false;
                if (!wantV) continue;
                const Vec3& v = ps.p[side];
                if (NearEnd(v, a, b) || SameXZ(v, exA) || SameXZ(v, exB)) continue;
                const float depth = fabsf(side == 0 ? c0 : c1) / len;
                if (depth > deepest) { deepest = depth; out.V = { v.x, v.z }; out.hasV = true; }
            }

            if (wantClear)
            {
                // 범위 밖 꼭짓점 (Portal ia-1, ib 끝점 + 양 끝 삼각형): 경계 상자(+여유) 밖이면 건너뜀
                const float cap = sqrtf(best2);
                const float minX = min(a.x, b.x) - cap, maxX = max(a.x, b.x) + cap;
                const float minZ = min(a.z, b.z) - cap, maxZ = max(a.z, b.z) + cap;
                auto extra = [&](const Vec3& v)
                {
                    if (v.x < minX || v.x > maxX || v.z < minZ || v.z > maxZ) return;
                    consider(v);
                };
                const int np = static_cast<int>(m_portals.size());
                for (int k : { ia - 1, ib })
                {
                    if (k < 0 || k >= np) continue;
                    for (int i = 0; i < 2; ++i)
                        if (m_portals[k].wall[i]) extra(m_portals[k].p[i]);
                }
                for (int t : { m_tp[ia], m_tp[ib] })
                {
                    const NavMeshTriangle& tri = m_nav.GetTriangle(t);
                    for (int i = 0; i < 3; ++i)
                        if (tri.boundaryVertex[i]) extra(tri.vertices[i]);
                }
                out.clear = sqrtf(best2);
            }
            else
            {
                out.clear = m_r;
            }
        }

        float GuideClear(int ia, int ib) const
        {
            if (ib <= ia) return m_r;
            float c = FLT_MAX;
            for (int k = ia; k < ib; ++k) c = min(c, m_guideClear[k]);
            return c;
        }

        void Push(const Vec3& p, int idx) { m_pts.push_back(p); m_idx.push_back(idx); }

        // Portal 위 경유점: V 가 끝점이면 V 에서 min(r, 변/2) (벽이 아니면 V 그대로), 아니면 중점
        //  (긴 Portal 에서 중점을 쓰면 크게 돌아가므로, 걸린 꼭짓점 쪽으로 붙인다)
        Vec3 PortalPoint(const PortalSeg& ps, const PtXZ* V) const
        {
            for (int iv = 0; V && iv < 2; ++iv)
            {
                if (!SameXZ(ps.p[iv], V, 1e-3f)) continue;
                const Vec3 d3 = ps.p[1 - iv] - ps.p[iv];
                const float ld = sqrtf(d3.x * d3.x + d3.z * d3.z);
                if (ld <= 1e-6f) break;
                const float off = ps.wall[iv] ? min(m_r, 0.5f * ld) : 0.f;
                return ps.p[iv] + d3 * (off / ld);
            }
            return (ps.p[0] + ps.p[1]) * 0.5f;
        }

        // 이웃 삼각형 점 두 개를 잇는다. Portal 을 못 지나면 Portal 위 경유점 (두 조각 모두 볼록 삼각형 안)
        void AddAdjacent(Vec3 p0, int i0, Vec3 p1, int i1)   // 값 복사: m_pts.back() 을 넘겨도 안전
        {
            if (i1 > i0)
            {
                ProbeResult pr;
                Probe(p0, i0, p1, i1, false, nullptr, nullptr, true, false, pr);
                if (!pr.inside)
                    Push(PortalPoint(m_portals[i1 - 1], (pr.hasV && i1 - i0 == 1) ? &pr.V : nullptr), i1);
            }
            Push(p1, i1);
        }

        void FollowGuide(int ia, const Vec3& b, int ib)
        {
            for (int k = ia + 1; k <= ib; ++k)
                AddAdjacent(m_pts.back(), m_idx.back(), (k < ib) ? m_raw[k] : b, k);
        }

        void Rec(Vec3 a, int ia, Vec3 b, int ib, int depth, const PtXZ* exA, const PtXZ* exB)
        {
            if (ib <= ia)
            {
                Push(b, ib);   // 같은 삼각형 안 (볼록) → 항상 통로 안
                return;
            }
            ProbeResult pr;
            Probe(a, ia, b, ib, true, exA, exB, true, true, pr);
            const float need = min(m_r, GuideClear(ia, ib)) - 1e-4f;
            if (pr.inside && pr.clear >= need)
            {
                Push(b, ib);
                return;
            }
            if (depth > 40 && ib - ia > 1)
            {
                FollowGuide(ia, b, ib);   // 너무 깊음 → 가이드(중점 경로) 그대로
                return;
            }
            PtXZ V = pr.V;
            bool hasV = pr.hasV;
            if (ib - ia == 1)
            {
                if (pr.inside) { Push(b, ib); return; }
                // 막힘 → a → Portal 위 경유점 → b (각 조각이 볼록 삼각형 하나 안). 걸린 꼭짓점 V 가 있으면 그 부채꼴 점
                Push(PortalPoint(m_portals[ia], hasV ? &V : nullptr), ib);
                Push(b, ib);
                return;
            }
            if (!hasV && pr.hasNearest)
            {
                // 통로 안이지만 여유 부족 → 가장 가까운 벽 꼭짓점을 V 로 (CorridorFan 의 '다각형 밖' 경우와 같음)
                V = pr.nearest;
                hasV = true;
            }
            if (!hasV)
            {
                FollowGuide(ia, b, ib);
                return;
            }

            // 부채꼴 점: V 를 끝점으로 갖는 범위 안 Portal 마다 V + min(r, 변/2)·(변 방향)
            //  연속한 두 부채꼴 점은 같은 삼각형의 두 변 위 → 잇는 선분은 판정 없이 통로 안
            //  [최적화] 점들은 공용 스택 m_fan 의 [f0, f1) 에 쌓는다 (재귀는 그 뒤에만 쌓고 끝나면 되돌림 → 복사 · 할당 없음)
            const PtXZ Vc = V;
            const size_t f0 = m_fan.size();
            for (int k = ia; k < ib; ++k)
            {
                const PortalSeg& ps = m_portals[k];
                int iv = -1;
                if (SameXZ(ps.p[0], &Vc, 1e-3f)) iv = 0;
                else if (SameXZ(ps.p[1], &Vc, 1e-3f)) iv = 1;
                if (iv < 0) continue;
                const Vec3& v3 = ps.p[iv];
                const Vec3 d3 = ps.p[1 - iv] - v3;
                const float ld = sqrtf(d3.x * d3.x + d3.z * d3.z);
                if (ld <= 1e-6f) continue;
                const float off = ps.wall[iv] ? min(m_r, 0.5f * ld) : 0.f;   // 벽이 아닌 꼭짓점은 여유 불필요 (Funnel 축소 규칙과 같음)
                m_fan.push_back({ v3 + d3 * (off / ld), k });
            }
            const size_t f1 = m_fan.size();
            if (f1 > f0)
            {
                const FanPt A = m_fan[f0];
                const FanPt B = m_fan[f1 - 1];
                Rec(a, ia, A.p, A.k, depth + 1, exA, &Vc);
                for (size_t i = f0; i < f1; ++i)
                {
                    const FanPt f = m_fan[i];   // 값 복사 (Push 가 m_fan 을 건드리진 않지만 재귀 뒤 재할당 대비)
                    if (LenXZ2(m_pts.back().x, m_pts.back().z, f.p.x, f.p.z) <= 1e-5f)
                    {
                        m_idx.back() = f.k + 1;   // 같은 점 (Portal f.k 위) → 다음 구간 기준 번호로
                        continue;
                    }
                    Push(f.p, f.k + 1);
                }
                Rec(B.p, B.k + 1, b, ib, depth + 1, &Vc, exB);
                m_fan.resize(f0);
                return;
            }

            // V 가 범위 Portal 끝점이 아님 (양 끝 삼각형의 세 번째 꼭짓점) → V 에 가장 가까운 중점에서 나눈다
            int k = ia + 1;
            float kd = FLT_MAX;
            for (int kk = ia + 1; kk < ib; ++kk)
            {
                const float d = LenXZ2(m_raw[kk].x, m_raw[kk].z, Vc.x, Vc.z);
                if (d < kd) { kd = d; k = kk; }
            }
            Rec(a, ia, m_raw[k], k, depth + 1, exA, nullptr);
            Rec(m_raw[k], k, b, ib, depth + 1, nullptr, exB);
        }

        // 먼 점부터 거꾸로: 통로 안 + 여유가 기존 구간들보다 나빠지지 않으면 건너뜀 (CorridorFan 의 SkipPass 와 같은 규칙)
        void SkipPass(float clearFactor)
        {
            const size_t n = m_pts.size();
            if (n <= 2) return;
            ProbeResult pr;
            m_segc.resize(n - 1);
            for (size_t k = 0; k + 1 < n; ++k)
            {
                Probe(m_pts[k], m_idx[k], m_pts[k + 1], m_idx[k + 1], false, nullptr, nullptr, false, true, pr);
                m_segc[k] = pr.clear;
            }

            m_tmpPts.clear(); m_tmpIdx.clear();
            m_tmpPts.push_back(m_pts[0]); m_tmpIdx.push_back(m_idx[0]);
            m_runMin.resize(n);
            size_t cur = 0;
            while (cur + 1 < n)
            {
                float rm = FLT_MAX;
                for (size_t j = cur + 1; j < n; ++j) { rm = min(rm, m_segc[j - 1]); m_runMin[j] = rm; }

                size_t next = cur + 1;
                for (size_t j = n - 1; j > cur + 1; --j)
                {
                    // 통로 판정 + 여유를 한 번에 (Probe 한 번)
                    Probe(m_pts[cur], m_idx[cur], m_pts[j], m_idx[j], false, nullptr, nullptr, false, true, pr);
                    if (pr.clear < min(m_r * clearFactor, m_runMin[j]) - 1e-4f)
                        continue;
                    if (pr.inside)
                    {
                        next = j;
                        break;
                    }
                }
                m_tmpPts.push_back(m_pts[next]); m_tmpIdx.push_back(m_idx[next]);
                cur = next;
            }
            m_pts.swap(m_tmpPts);
            m_idx.swap(m_tmpIdx);
        }

        const NavMesh& m_nav;
        const vector<Vec3>& m_raw;
        const vector<int>& m_tp;
        float m_r;
        vector<PortalSeg> m_portals;   // m_portals[k] = tp[k] ↔ tp[k+1] 공유 변
        vector<float> m_guideClear;    // 중점 경로 구간별 벽 여유 (r 에서 자름)
        vector<Vec3> m_pts;
        vector<int> m_idx;             // 점이 든 삼각형의 통로 번호 (tp 인덱스). Portal 위 점은 다음 삼각형 번호
        // 재사용 버퍼
        vector<float> m_segc, m_runMin;
        vector<Vec3> m_tmpPts;
        vector<int> m_tmpIdx;
        struct FanPt { Vec3 p; int k; };   // k = Portal 번호 (tp[k] ↔ tp[k+1])
        vector<FanPt> m_fan;               // 부채꼴 점 스택 (재귀 단계마다 [f0, f1))
    };
}

void NavMesh::SmoothPathCorridorFanPortal(const vector<Vec3>& rawPath, const vector<int>& trianglePath, vector<Vec3>& outPath, vector<int>& outTris)
{
    outPath.clear();
    outTris.clear();
    if (rawPath.size() <= 2 || trianglePath.size() != rawPath.size())
    {
        SmoothPathCorridor(rawPath, trianglePath, outPath, outTris);
        return;
    }
    PortalFanSplitter splitter(*this, rawPath, trianglePath, m_agentRadius);
    if (!splitter.Build())
    {
        SmoothPathCorridorSplit(rawPath, trianglePath, outPath, outTris, true);   // 변 이웃이 비어 있는 통로 (드묾)
        return;
    }
    splitter.Run(outPath, outTris);
}

// =============================================================================
// 최단 조합 스무딩 (CorridorDP)
//  후보는 Corridor 와 같다 (출발 + 삼각형 중점들 + 도착), 판정도 같다 (통로 삼각형 따라 걷기).
//  다른 점: '현재 점에서 보이는 가장 먼 점'(탐욕) 대신, 이을 수 있는 점들 중 '총 길이가 최소인 순서'를 고른다.
//  → 탐욕은 멀리 보이는 점까지 직진했다가 크게 꺾지만, DP 는 코너 근처 중점을 거쳐 미리 꺾는다.
//  best[i] = 출발 → 후보 i 까지 최소 길이,  best[i] = min_j ( best[j] + |j-i| )  (j→i 가 통로 안일 때)
//  바로 앞 점(i-1)과도 못 이으면(오목한 코너) 공유 변 중점을 거쳐 잇는다 (삼각형은 볼록 → 항상 통로 안)
// =============================================================================
void NavMesh::SmoothPathCorridorDP(const vector<Vec3>& rawPath, const vector<int>& trianglePath, vector<Vec3>& outPath, vector<int>& outTris)
{
    outPath.clear();
    outTris.clear();
    const int n = static_cast<int>(rawPath.size());
    if (n <= 2 || static_cast<int>(trianglePath.size()) != n)
    {
        SmoothPathCorridor(rawPath, trianglePath, outPath, outTris);
        return;
    }

    vector<float> best(n, FLT_MAX);
    vector<int> parent(n, -1);
    vector<uint8> viaMid(n, 0);       // parent → i 를 공유 변 중점을 거쳐 이었는가
    vector<Vec3> midPoint(n);
    best[0] = 0.f;

    for (int i = 1; i < n; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (best[j] == FLT_MAX)
                continue;
            const float d = Vec3::Distance(rawPath[j], rawPath[i]);
            if (best[j] + d >= best[i])
                continue;   // 이어도 더 짧아지지 않음 → 판정 생략 (가지치기)

            if (IsSegmentInCorridor(rawPath[j], trianglePath[j], rawPath[i], trianglePath[i], m_corridorMask))
            {
                best[i] = best[j] + d;
                parent[i] = j;
                viaMid[i] = 0;
            }
            else if (j == i - 1)
            {
                // 바로 옆 중점끼리도 못 이음 → 공유 변 중점 경유
                const NavMeshTriangle& tri = m_triangles[trianglePath[j]];
                for (int e = 0; e < 3; ++e)
                {
                    if (tri.edgeNeighbor[e] != trianglePath[i])
                        continue;
                    const Vec3 mid = (tri.vertices[e] + tri.vertices[(e + 1) % 3]) * 0.5f;
                    const float dm = best[j] + Vec3::Distance(rawPath[j], mid) + Vec3::Distance(mid, rawPath[i]);
                    if (dm < best[i])
                    {
                        best[i] = dm;
                        parent[i] = j;
                        viaMid[i] = 1;
                        midPoint[i] = mid;
                    }
                    break;
                }
            }
        }
    }

    if (parent[n - 1] == -1)
    {
        // 이론상 일어나지 않음 (바로 옆 점은 항상 이어짐). 안전하게 탐욕 Corridor 로 대체
        SmoothPathCorridor(rawPath, trianglePath, outPath, outTris);
        return;
    }

    // 도착점에서 거꾸로 따라가며 복원
    vector<int> order;
    for (int k = n - 1; k != -1; k = parent[k])
        order.push_back(k);
    outPath.reserve(order.size() * 2);
    outTris.reserve(order.size() * 2);
    for (auto it = order.rbegin(); it != order.rend(); ++it)
    {
        const int k = *it;
        if (viaMid[k])
        {
            outPath.push_back(midPoint[k]);
            outTris.push_back(trianglePath[k]);
        }
        outPath.push_back(rawPath[k]);
        outTris.push_back(trianglePath[k]);
    }
}

// =============================================================================
// 코너 당기기 (CorridorPull)
//  Corridor 결과에서 남은 중간 점 = 시야가 끊겨 꺾이는 곳.
//  그 점을 (이전 점, 현재 점, 다음 점) 삼각형 안의 가장 깊은 벽 꼭짓점 V 쪽으로 당겨
//  V 에서 반경 r 떨어진 곳에 둔다 → 길이는 Funnel 쪽으로, 벽 여유는 r 로 조절.
//  당긴 점과 앞뒤 선분은 모두 TraceSegmentInCorridor 로 검증하고, 실패하면 절반씩 덜 당긴다.
// =============================================================================
int NavMesh::FindCornerBoundaryVertex(const vector<int>& trianglePath, const Vec3& prev, const Vec3& cur, const Vec3& next, Vec3& outVertex) const
{
    // (prev, cur, next) 삼각형의 방향. 거의 일직선이면 꺾이는 코너가 없다
    const float abx = cur.x - prev.x, abz = cur.z - prev.z;
    const float acx = next.x - prev.x, acz = next.z - prev.z;
    const float orient = Cross2D(abx, abz, acx, acz);
    const float baseLen = sqrtf(acx * acx + acz * acz);
    if (baseLen < 1e-4f || fabsf(orient) < 1e-4f * baseLen)
        return -1;
    const float sg = orient > 0.f ? 1.f : -1.f;
    const float eps = 1e-4f;

    int best = -1;
    float bestDepth = 0.f;
    for (int ti : trianglePath)
    {
        const NavMeshTriangle& tri = m_triangles[ti];
        for (int k = 0; k < 3; ++k)
        {
            if (!tri.boundaryVertex[k])
                continue;
            const Vec3& v = tri.vertices[k];
            // v 가 (prev, cur, next) 삼각형 안 (XZ) 인가
            if (Cross2D(cur.x - prev.x, cur.z - prev.z, v.x - prev.x, v.z - prev.z) * sg < -eps) continue;
            if (Cross2D(next.x - cur.x, next.z - cur.z, v.x - cur.x, v.z - cur.z) * sg < -eps) continue;
            if (Cross2D(prev.x - next.x, prev.z - next.z, v.x - next.x, v.z - next.z) * sg < -eps) continue;
            // 가장 깊은 = 선분 prev-next 에서 가장 먼 꼭짓점 (줄을 당기면 걸리는 점)
            const float depth = fabsf(Cross2D(acx, acz, v.x - prev.x, v.z - prev.z)) / baseLen;
            if (depth > bestDepth)
            {
                bestDepth = depth;
                best = ti;
                outVertex = v;
            }
        }
    }
    return best;
}

void NavMesh::PullCornersToWall(const vector<int>& trianglePath, vector<Vec3>& path, vector<int>& pathTris) const
{
    if (path.size() < 3 || pathTris.size() != path.size() || m_agentRadius <= 0.f)
        return;

    for (size_t i = 1; i + 1 < path.size(); ++i)
    {
        const Vec3 prev = path[i - 1], cur = path[i], next = path[i + 1];
        const int prevTri = pathTris[i - 1], curTri = pathTris[i], nextTri = pathTris[i + 1];
        if (prevTri < 0 || curTri < 0 || nextTri < 0)
            continue;

        Vec3 v;
        if (FindCornerBoundaryVertex(trianglePath, prev, cur, next, v) < 0)
            continue;

        // 목표: V 에서 cur 방향으로 반경 r 떨어진 점
        const float dx = cur.x - v.x, dz = cur.z - v.z;
        const float d = sqrtf(dx * dx + dz * dz);
        if (d <= m_agentRadius + 1e-3f)
            continue;   // 이미 반경 안쪽까지 붙어 있음
        const float fullX = v.x + dx / d * m_agentRadius;
        const float fullZ = v.z + dz / d * m_agentRadius;

        // 1 → 1/2 → 1/4 만큼 당겨 보고 처음 통과하는 것을 쓴다
        float pull = 1.f;
        for (int attempt = 0; attempt < 3; ++attempt, pull *= 0.5f)
        {
            Vec3 q = cur;
            q.x = cur.x + (fullX - cur.x) * pull;
            q.z = cur.z + (fullZ - cur.z) * pull;

            int qTri = -1;
            if (!TraceSegmentInCorridor(cur, curTri, q, m_corridorMask, -1, qTri) || qTri < 0)
                continue;   // 당긴 점이 통로 밖
            if (!IsSegmentInCorridor(prev, prevTri, q, qTri, m_corridorMask))
                continue;
            if (!IsSegmentInCorridor(q, qTri, next, nextTri, m_corridorMask))
                continue;

            q.y = HeightOnTriangle(m_triangles[qTri], q.x, q.z, cur.y);
            path[i] = q;
            pathTris[i] = qTri;
            break;
        }
    }
}

// =============================================================================
// Funnel (String Pulling) - Simple Stupid Funnel Algorithm 방식
//  1) A* 삼각형 목록 → 이웃 삼각형끼리 공유하는 변(Portal) 목록 (left/right 정렬)
//  2) apex / left / right 깔때기를 Portal 순서대로 좁히다가
//     한쪽이 반대편 경계를 넘으면 그 반대편 점을 코너로 확정하고 거기서 다시 시작
//  계산은 XZ 평면에서 한다.
// =============================================================================
namespace
{
    // (c - a) x (b - a) in XZ. 부호로 "c 가 a->b 의 어느 쪽인가"를 판정한다.
    // Portal left/right 정렬과 깔때기 갱신에 같은 함수를 써서 좌표계와 무관하게 일관성을 맞춘다.
    float TriArea2XZ(const Vec3& a, const Vec3& b, const Vec3& c)
    {
        const float abx = b.x - a.x, abz = b.z - a.z;
        const float acx = c.x - a.x, acz = c.z - a.z;
        return acx * abz - abx * acz;
    }

    bool SamePointXZ(const Vec3& a, const Vec3& b)
    {
        const float dx = a.x - b.x, dz = a.z - b.z;
        return dx * dx + dz * dz < 1e-6f;
    }
}

const char* NavMesh::SmoothingModeName(NavSmoothingMode mode)
{
    switch (mode)
    {
    case NavSmoothingMode::LOS:      return "LOS";
    case NavSmoothingMode::Corridor: return "Corridor";
    case NavSmoothingMode::CorridorPull: return "CorridorPull";
    case NavSmoothingMode::CorridorDP: return "CorridorDP";
    case NavSmoothingMode::CorridorSplit: return "CorridorSplit";
    case NavSmoothingMode::CorridorFan: return "CorridorFan";
    case NavSmoothingMode::CorridorFanPortal: return "CorridorFanPortal";
    case NavSmoothingMode::Funnel:   return "Funnel";
    case NavSmoothingMode::FunnelFan: return "FunnelFan";
    case NavSmoothingMode::FunnelCurve: return "FunnelCurve";
    case NavSmoothingMode::FunnelCurveAngle: return "FunnelCurveAngle";
    case NavSmoothingMode::FunnelCurveTangent: return "FunnelCurveTangent";
    default:                         return "?";
    }
}

void NavMesh::BuildPortals(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<pair<Vec3, Vec3>>& outPortals, vector<NavPortalInfo>* outInfo, float shrinkRadius) const
{
    const float radius = (shrinkRadius >= 0.f) ? shrinkRadius : m_agentRadius;
    outPortals.clear();
    outPortals.reserve(trianglePath.size() + 1);
    outPortals.emplace_back(start, start);
    if (outInfo)
    {
        outInfo->clear();
        outInfo->push_back({ start, start, false, false, trianglePath.empty() ? -1 : trianglePath.front() });
    }

    for (size_t i = 0; i + 1 < trianglePath.size(); ++i)
    {
        const NavMeshTriangle& cur = m_triangles[trianglePath[i]];
        const int next = trianglePath[i + 1];

        Vec3 a, b;
        bool aBoundary = false, bBoundary = false;
        bool found = false;
        for (int e = 0; e < 3; ++e)
        {
            if (cur.edgeNeighbor[e] == next)
            {
                a = cur.vertices[e];
                b = cur.vertices[(e + 1) % 3];
                aBoundary = cur.boundaryVertex[e];
                bBoundary = cur.boundaryVertex[(e + 1) % 3];
                found = true;
                break;
            }
        }

        if (!found)
        {
            // 변 단위 이웃이 비어 있으면 공유 꼭짓점 2개를 거리로 찾는다
            const NavMeshTriangle& nt = m_triangles[next];
            int count = 0;
            int sharedIdx[2] = { -1, -1 };
            for (int k = 0; k < 3 && count < 2; ++k)
            {
                for (int m = 0; m < 3; ++m)
                {
                    if (Vec3::Distance(cur.vertices[k], nt.vertices[m]) < 0.01f)
                    {
                        sharedIdx[count++] = k;
                        break;
                    }
                }
            }
            if (count < 2)
                continue;
            a = cur.vertices[sharedIdx[0]];
            b = cur.vertices[sharedIdx[1]];
            aBoundary = cur.boundaryVertex[sharedIdx[0]];
            bBoundary = cur.boundaryVertex[sharedIdx[1]];
        }

        // 현재 삼각형 중심에서 이 변을 바라볼 때: a 기준 b 가 오른쪽이면 (left=a, right=b)
        Vec3 left = a, right = b;
        bool leftBoundary = aBoundary, rightBoundary = bBoundary;
        if (TriArea2XZ(cur.center, a, b) <= 0.f)
        {
            std::swap(left, right);
            std::swap(leftBoundary, rightBoundary);
        }

        if (outInfo)
            outInfo->push_back({ left, right, leftBoundary, rightBoundary, next });

        // F1: 벽(경계) 위 꼭짓점 쪽 끝만 Agent 반경만큼 안쪽으로 줄인다.
        //     Portal 이 너무 짧으면 두 끝이 같은 점에서 만난다 (= 통과 가능한 한 점)
        if (radius > 0.f)
        {
            const Vec3 d = right - left;
            const float len = sqrtf(d.x * d.x + d.z * d.z);
            const float shrinkL = leftBoundary ? radius : 0.f;
            const float shrinkR = rightBoundary ? radius : 0.f;
            if (len > 1e-6f && shrinkL + shrinkR > 0.f)
            {
                const float k = min(1.f, len / (shrinkL + shrinkR));
                const Vec3 newLeft = left + d * (shrinkL * k / len);
                const Vec3 newRight = right - d * (shrinkR * k / len);
                left = newLeft;
                right = newRight;
            }
        }

        outPortals.emplace_back(left, right);
    }

    outPortals.emplace_back(end, end);
    if (outInfo)
        outInfo->push_back({ end, end, false, false, trianglePath.empty() ? -1 : trianglePath.back() });
}

void NavMesh::StringPull(const vector<pair<Vec3, Vec3>>& portals, vector<Vec3>& outPath, vector<pair<int, int>>* outCorners) const
{
    outPath.clear();
    if (outCorners) outCorners->clear();
    if (portals.empty())
        return;

    Vec3 apex = portals[0].first;
    Vec3 left = portals[0].first;
    Vec3 right = portals[0].second;
    int apexIndex = 0, leftIndex = 0, rightIndex = 0;

    outPath.push_back(apex);

    const int n = static_cast<int>(portals.size());
    const int maxIterations = n * n + 16;   // 재시작이 반복돼도 무한 루프 방지
    int iterations = 0;

    for (int i = 1; i < n && iterations < maxIterations; ++i, ++iterations)
    {
        const Vec3& newLeft = portals[i].first;
        const Vec3& newRight = portals[i].second;

        // ---- right 갱신 ----
        if (TriArea2XZ(apex, right, newRight) <= 0.f)
        {
            if (SamePointXZ(apex, right) || TriArea2XZ(apex, left, newRight) > 0.f)
            {
                right = newRight;          // 좁히기
                rightIndex = i;
            }
            else
            {
                // right 가 left 를 넘음 → left 가 코너
                if (!SamePointXZ(outPath.back(), left))
                {
                    outPath.push_back(left);
                    if (outCorners) outCorners->emplace_back(leftIndex, 0);
                }
                apex = left;
                apexIndex = leftIndex;
                left = right = apex;
                leftIndex = rightIndex = apexIndex;
                i = apexIndex;             // 루프의 ++i 로 apex 다음 Portal 부터 다시
                continue;
            }
        }

        // ---- left 갱신 ----
        if (TriArea2XZ(apex, left, newLeft) >= 0.f)
        {
            if (SamePointXZ(apex, left) || TriArea2XZ(apex, right, newLeft) < 0.f)
            {
                left = newLeft;            // 좁히기
                leftIndex = i;
            }
            else
            {
                // left 가 right 를 넘음 → right 가 코너
                if (!SamePointXZ(outPath.back(), right))
                {
                    outPath.push_back(right);
                    if (outCorners) outCorners->emplace_back(rightIndex, 1);
                }
                apex = right;
                apexIndex = rightIndex;
                left = right = apex;
                leftIndex = rightIndex = apexIndex;
                i = apexIndex;
                continue;
            }
        }
    }

    // 도착점 (중복 추가 방지)
    const Vec3& goal = portals.back().first;
    if (SamePointXZ(outPath.back(), goal))
    {
        outPath.back() = goal;
        if (outCorners && outPath.size() >= 2 && outCorners->size() + 1 == outPath.size())
            outCorners->pop_back();   // 마지막 코너가 도착점과 겹침 → 도착점으로 대체
    }
    else
        outPath.push_back(goal);
}

// =============================================================================
// FunnelFan: 코너 찾기는 Funnel (Portal 한 번 훑기, O(n)), 코너를 도는 방식만 부채꼴 점으로 바꾼다
//  Funnel 은 코너 꼭짓점 V 근처 Portal 하나 위 점(반경 축소 점)에서 한 번에 꺾는다 → U 자 코너에서 150° 이상 급회전
//  여기서는 V 를 끝점으로 갖는 연속 Portal 마다 V + min(r, 변/2)·(변 방향) (CorridorFan 의 부채꼴 점) → 반지름 r 원호 근사
//   - 줄이 닿지 않는 앞뒤 점은 볼록 껍질 규칙으로 뺀다 (prev → 점들 → next 가 코너 방향으로만 꺾이게)
//   - 같은 꼭짓점 V 에 연달아 찍힌 Funnel 코너(반경 축소 점이 Portal 마다 달라 생김)는 하나로 묶어 부채꼴을 한 번만 만든다
//   - 새로 생긴 선분만 통로 판정 (코너당 몇 번). 실패하면 그 코너는 Funnel 점 그대로
//  불변식: 출력의 연속한 두 점은 항상 통로 판정을 통과한 선분 (또는 Funnel 원래 선분)
// =============================================================================
void NavMesh::SmoothPathFunnelFan(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath, vector<pair<Vec3, Vec3>>& outPortals)
{
    outPath.clear();
    vector<NavPortalInfo> info;
    BuildPortals(trianglePath, start, end, outPortals, &info);
    vector<Vec3> pulled;
    vector<pair<int, int>> corners;   // pulled[c + 1] 의 (Portal 번호, 0 = left / 1 = right)
    StringPull(outPortals, pulled, &corners);
    const float r = m_agentRadius;
    if (pulled.size() < 3 || corners.size() + 2 != pulled.size() || info.size() != outPortals.size() || r <= 0.f)
    {
        outPath = pulled;
        return;
    }

    const int np = static_cast<int>(info.size());
    const size_t nc = corners.size();
    auto validPortal = [&](int pi) { return pi >= 1 && pi <= np - 2; };
    auto cornerV = [&](size_t k) -> const Vec3& { return (corners[k].second == 0) ? info[corners[k].first].left : info[corners[k].first].right; };

    outPath.reserve(pulled.size() * 3);
    outPath.push_back(pulled[0]);
    int prevTri = trianglePath.front();
    vector<Vec3> fan, hull;
    vector<int> fanTri, hullTri;

    size_t c = 0;
    while (c < nc)
    {
        const int pi = corners[c].first;
        const int side = corners[c].second;

        // 같은 꼭짓점 V 에 연달아 찍힌 코너를 하나로 묶는다.
        //  반경 축소 끝점이 Portal 마다 조금씩 달라 Funnel 은 V 하나에 코너를 여러 개 (0.02~0.03 간격) 찍는다.
        //  묶지 않으면 코너마다 부채꼴을 따로 만들어 부채꼴이 겹치고 앞뒤로 왔다 갔다 한다.
        size_t c2 = c;
        if (validPortal(pi))
        {
            while (c2 + 1 < nc && validPortal(corners[c2 + 1].first) && corners[c2 + 1].second == side
                && SamePointXZ(cornerV(c2 + 1), cornerV(c)))
                ++c2;
        }
        const Vec3& next = pulled[c2 + 2];
        const int nextTri = (c2 + 1 < nc) ? info[corners[c2 + 1].first].tri : trianglePath.back();
        const Vec3 prev = outPath.back();
        const int lastTri = info[max(0, min(np - 1, corners[c2].first))].tri;
        auto keepApex = [&]()
        {
            for (size_t k = c; k <= c2; ++k)
                outPath.push_back(pulled[k + 1]);
            prevTri = lastTri;
        };

        if (!validPortal(pi))
        {
            keepApex();
            c = c2 + 1;
            continue;
        }

        const Vec3 V = cornerV(c);
        const bool wallV = (side == 0) ? info[pi].leftWall : info[pi].rightWall;
        const float turn = Cross2D(V.x - prev.x, V.z - prev.z, next.x - V.x, next.z - V.z);
        if (!wallV || fabsf(turn) < 1e-8f)
        {
            keepApex();   // 벽이 아닌 꼭짓점(여유 불필요) / 거의 직진
            c = c2 + 1;
            continue;
        }
        const float sg = (turn > 0.f) ? 1.f : -1.f;

        // V 를 끝점으로 갖는 연속 Portal [j0, j1] (묶은 코너 앞뒤로 넓힘)
        auto hasV = [&](int j) { return SamePointXZ(info[j].left, V) || SamePointXZ(info[j].right, V); };
        int j0 = pi, j1 = corners[c2].first;
        while (j0 - 1 >= 1 && hasV(j0 - 1)) --j0;
        while (j1 + 1 <= np - 2 && hasV(j1 + 1)) ++j1;

        fan.clear(); fanTri.clear();
        for (int j = j0; j <= j1; ++j)
        {
            if (!hasV(j)) continue;
            const bool vIsLeft = SamePointXZ(info[j].left, V);
            const Vec3& O = vIsLeft ? info[j].right : info[j].left;
            const Vec3 d = O - V;
            const float ld = sqrtf(d.x * d.x + d.z * d.z);
            if (ld <= 1e-6f) continue;
            fan.push_back(V + d * (min(r, 0.5f * ld) / ld));
            fanTri.push_back(info[j].tri);
        }

        // 볼록 껍질: prev → 부채꼴 점들 → next 가 sg 방향으로만 꺾이도록, 반대로 꺾이게 만드는 점은 뺀다
        hull.clear(); hullTri.clear();
        hull.push_back(prev); hullTri.push_back(prevTri);
        auto pushHull = [&](const Vec3& p, int t)
        {
            while (hull.size() >= 2)
            {
                const Vec3& a = hull[hull.size() - 2];
                const Vec3& b = hull.back();
                if (Cross2D(b.x - a.x, b.z - a.z, p.x - b.x, p.z - b.z) * sg > 1e-6f)
                    break;
                hull.pop_back(); hullTri.pop_back();
            }
            hull.push_back(p); hullTri.push_back(t);
        };
        for (size_t i = 0; i < fan.size(); ++i)
            pushHull(fan[i], fanTri[i]);
        pushHull(next, nextTri);

        // 새 선분 통로 판정 (prev → … → next). 하나라도 실패하면 Funnel 점 그대로
        bool ok = hull.size() >= 3;
        for (size_t i = 0; i + 1 < hull.size() && ok; ++i)
            ok = IsSegmentInCorridor(hull[i], hullTri[i], hull[i + 1], hullTri[i + 1], m_corridorMask);
        if (!ok)
            keepApex();
        else
        {
            for (size_t i = 1; i + 1 < hull.size(); ++i)
                outPath.push_back(hull[i]);
            prevTri = hullTri[hull.size() - 2];
        }
        c = c2 + 1;
    }
    outPath.push_back(pulled.back());
}

// =============================================================================
// FunnelCurve: Funnel 코너를 3차 Bezier 곡선으로 돈다 (경로 자체가 곡선 → LookAhead 없이도 부드럽고, 모든 Agent 에 적용)
//  1) Funnel 을 반경 R = 1.5r 로 축소해 코너를 V 에서 넉넉히 띄운다
//     (곡선은 항상 코너보다 안쪽 = V 쪽으로 지나가므로, 파고들 여유를 미리 둔다. R = r 이면 거의 모든 코너가 검증에 실패)
//  2) 같은 꼭짓점 V 에 몰린 코너는 묶는다. 진입점 T1 = A1 - d·진입방향, 진출점 T2 = A2 + d·진출방향
//     (d ≤ 1, 앞뒤 선분의 절반 이하 → 이웃 코너의 곡선과 겹치지 않는다)
//  3) Bezier [T1, T1 + 2/3(A1 - T1), T2 + 2/3(A2 - T2), T2]: 양 끝 방향이 직선과 같아 직선 → 곡선 → 직선이 끊기지 않는다
//  4) 회전 15° 마다 샘플 하나. 검증: 샘플 선분이 V 에서 r 이상 + 통로 판정
//     실패하면 d 를 반으로 (최대 3번), 그래도 실패면 그 코너는 Funnel 점 그대로
//  오프라인 (998쌍, navdiag_bezier.py): 회전 중앙값 37.7° → 16.0°, 벽 여유 중앙값 0.142 → 0.243, 길이 +0.23%
// =============================================================================
namespace
{
    constexpr float FUNNEL_CURVE_RADIUS_SCALE = 1.5f;   // 코너 축소 반경 = r × 1.5
    constexpr float FUNNEL_CURVE_DIST = 1.0f;           // 코너에서 진입점 / 진출점까지 최대 거리
    constexpr float FUNNEL_CURVE_STEP_DEG = 15.f;       // 샘플 하나당 회전
    constexpr float FUNNEL_CURVE_MIN_TURN_DEG = 8.f;    // 이보다 작게 꺾이면 곡선 없이 그대로
    constexpr float FUNNEL_CURVE_MIN_SEG = 0.12f;       // FunnelCurveAngle: 이보다 가까운 점은 뺀다 (도착 판정 0.1 보다 촘촘하면 이동 방향이 튄다)
    constexpr float FUNNEL_CURVE_KINK_DEG = 20.f;       // FunnelCurveAngle: 고정 점에서 원래보다 이만큼 더 꺾이면 이웃 코너도 고정

    float CurveSegDistXZ(const Vec3& p, const Vec3& a, const Vec3& b)
    {
        const float abx = b.x - a.x, abz = b.z - a.z;
        const float len2 = abx * abx + abz * abz;
        float t = (len2 > 1e-12f) ? ((p.x - a.x) * abx + (p.z - a.z) * abz) / len2 : 0.f;
        t = min(1.f, max(0.f, t));
        const float qx = a.x + abx * t - p.x, qz = a.z + abz * t - p.z;
        return sqrtf(qx * qx + qz * qz);
    }
}

void NavMesh::SmoothPathFunnelCurve(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath, vector<pair<Vec3, Vec3>>& outPortals)
{
    outPath.clear();
    const float r = m_agentRadius;
    vector<NavPortalInfo> info;
    BuildPortals(trianglePath, start, end, outPortals, &info, r * FUNNEL_CURVE_RADIUS_SCALE);
    vector<Vec3> pulled;
    vector<pair<int, int>> corners;   // pulled[c + 1] 의 (Portal 번호, 0 = left / 1 = right)
    StringPull(outPortals, pulled, &corners);
    if (pulled.size() < 3 || corners.size() + 2 != pulled.size() || info.size() != outPortals.size() || r <= 0.f)
    {
        outPath = pulled;
        return;
    }

    const int np = static_cast<int>(info.size());
    const size_t nc = corners.size();
    auto validPortal = [&](int pi) { return pi >= 1 && pi <= np - 2; };
    auto cornerV = [&](size_t k) -> const Vec3& { return (corners[k].second == 0) ? info[corners[k].first].left : info[corners[k].first].right; };

    // 코너 묶음 (같은 V 에 연달아 찍힌 코너)
    struct CornerGroup { size_t c0, c1; bool curve; Vec3 V; };
    vector<CornerGroup> groups;
    groups.reserve(nc);
    for (size_t c = 0; c < nc;)
    {
        size_t c2 = c;
        bool curve = false;
        Vec3 V = pulled[c + 1];
        const int pi = corners[c].first;
        const int side = corners[c].second;
        if (validPortal(pi))
        {
            while (c2 + 1 < nc && validPortal(corners[c2 + 1].first) && corners[c2 + 1].second == side
                && SamePointXZ(cornerV(c2 + 1), cornerV(c)))
                ++c2;
            V = cornerV(c);
            curve = (side == 0) ? info[pi].leftWall : info[pi].rightWall;   // 벽 꼭짓점만 (벽이 아니면 여유가 필요 없다)
        }
        groups.push_back({ c, c2, curve, V });
        c = c2 + 1;
    }

    outPath.reserve(pulled.size() * 4);
    outPath.push_back(pulled[0]);
    vector<Vec3> pts;
    const size_t G = groups.size();
    for (size_t gi = 0; gi < G; ++gi)
    {
        const CornerGroup& g = groups[gi];
        const Vec3& A1 = pulled[g.c0 + 1];
        const Vec3& A2 = pulled[g.c1 + 1];
        auto keepCorner = [&]()
        {
            for (size_t k = g.c0; k <= g.c1; ++k)
                outPath.push_back(pulled[k + 1]);
        };
        if (!g.curve)
        {
            keepCorner();
            continue;
        }

        const Vec3& prevP = pulled[g.c0];
        const Vec3& nextP = pulled[g.c1 + 2];
        float inX = A1.x - prevP.x, inZ = A1.z - prevP.z;
        float outX = nextP.x - A2.x, outZ = nextP.z - A2.z;
        const float lin = sqrtf(inX * inX + inZ * inZ);
        const float lout = sqrtf(outX * outX + outZ * outZ);
        if (lin < 1e-5f || lout < 1e-5f)
        {
            keepCorner();
            continue;
        }
        inX /= lin; inZ /= lin; outX /= lout; outZ /= lout;
        const float turnDeg = acosf(max(-1.f, min(1.f, inX * outX + inZ * outZ))) * 57.29578f;
        if (turnDeg < FUNNEL_CURVE_MIN_TURN_DEG)
        {
            keepCorner();
            continue;
        }

        // d: 앞뒤 선분의 절반까지 (출발 / 도착 선분은 0.9), 앞 코너 곡선이 끝난 점보다 앞으로는 가지 않게
        const Vec3& last = outPath.back();
        float limIn = lin * ((gi == 0) ? 0.9f : 0.5f);
        const float limOut = lout * ((gi + 1 == G) ? 0.9f : 0.5f);
        const float toLast = sqrtf((A1.x - last.x) * (A1.x - last.x) + (A1.z - last.z) * (A1.z - last.z));
        limIn = min(limIn, toLast * 0.999f);
        float d = min(FUNNEL_CURVE_DIST, min(limIn, limOut));
        const int n = max(2, static_cast<int>(ceilf(turnDeg / FUNNEL_CURVE_STEP_DEG)));

        bool ok = false;
        for (int attempt = 0; attempt < 4 && !ok && d > 1e-4f; ++attempt, d *= 0.5f)
        {
            const Vec3 T1 = A1 + (prevP - A1) * (d / lin);
            const Vec3 T2 = A2 + (nextP - A2) * (d / lout);
            const Vec3 P1 = T1 + (A1 - T1) * (2.f / 3.f);
            const Vec3 P2 = T2 + (A2 - T2) * (2.f / 3.f);
            pts.clear();
            pts.push_back(T1);
            for (int i = 1; i < n; ++i)
            {
                const float t = static_cast<float>(i) / n, u = 1.f - t;
                pts.push_back(T1 * (u * u * u) + P1 * (3.f * u * u * t) + P2 * (3.f * u * t * t) + T2 * (t * t * t));
            }
            pts.push_back(T2);

            // 검증 1: 꼭짓점 V 에서 r 이상 (곡선이 코너 안쪽으로 파고드는 깊이 확인)
            bool good = true;
            for (size_t i = 0; i + 1 < pts.size() && good; ++i)
                good = CurveSegDistXZ(g.V, pts[i], pts[i + 1]) >= r - 1e-4f;
            // 검증 2: 통로 판정 (T1 은 원래 선분 위라 통로 안. prev → T1, T2 → next 는 원래 선분의 일부라 따로 볼 필요 없음)
            if (good)
            {
                int cur = FindTriangleContaining(pts[0]);
                if (cur < 0 || !m_corridorMask[cur])
                    good = false;
                for (size_t i = 0; i + 1 < pts.size() && good; ++i)
                {
                    int endTri = -1;
                    good = TraceSegmentInCorridor(pts[i], cur, pts[i + 1], m_corridorMask, -1, endTri);
                    cur = endTri;
                }
            }
            ok = good;
        }
        if (ok)
            outPath.insert(outPath.end(), pts.begin(), pts.end());
        else
            keepCorner();
    }
    outPath.push_back(pulled.back());

    // 같은 점 제거 (도착점은 유지)
    size_t w = 1;
    for (size_t i = 1; i < outPath.size(); ++i)
    {
        if (SamePointXZ(outPath[i], outPath[w - 1]) && i + 1 < outPath.size())
            continue;
        outPath[w++] = outPath[i];
    }
    outPath.resize(w);
}

// =============================================================================
// FunnelCurveAngle: FunnelCurve 에서 '반으로 줄이며 시도' 를 없애고 각도로 바로 계산
//  1) 코너 기준점 A 를 꼭짓점 V 의 안쪽 이등분선 위로 옮긴다: A' = V - n·R (n = 코너 안쪽 방향) → φ = 0
//     (A 를 옮기면 앞뒤 방향이 바뀌므로 이웃 코너와 함께 2번 갱신)
//  2) d = min(1, 앞뒤 선분 절반, 2(R - r) / sin(θ/2))
//     V 가 이등분선 위면 곡선의 가장 깊은 점은 한가운데 (t = ½) 이고 깊이는 (d/2)·sin(θ/2) 이므로
//     V 까지 거리 = R - (d/2)·sin(θ/2) ≥ r  → 곡선 전체가 V 에서 r 이상 (식으로 보장)
//     앞뒤 직선은 V 에 가장 가까운 지점이 코너 너머(곡선으로 바뀐 부분)라, T1 · T2 보다 V 에서 멀다
//  3) 남은 확인은 통로 판정뿐 (다른 벽). 모든 선분을 판정하고, 실패한 코너는 한 단계씩 내린다
//     (옮긴 곡선 → 원래 Funnel 점에서 곡선 → Funnel 점) 뒤 이웃을 다시 계산. 고정 점에서 크게 꺾이면 이웃도 내린다
// =============================================================================
void NavMesh::SmoothPathFunnelCurveAngle(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath, vector<pair<Vec3, Vec3>>& outPortals)
{
    outPath.clear();
    const float r = m_agentRadius;
    const float R = r * FUNNEL_CURVE_RADIUS_SCALE;
    vector<NavPortalInfo> info;
    BuildPortals(trianglePath, start, end, outPortals, &info, R);
    vector<Vec3> pulled;
    vector<pair<int, int>> corners;
    StringPull(outPortals, pulled, &corners);
    if (pulled.size() < 3 || corners.size() + 2 != pulled.size() || info.size() != outPortals.size() || r <= 0.f)
    {
        outPath = pulled;
        return;
    }

    const int np = static_cast<int>(info.size());
    const size_t nc = corners.size();
    auto validPortal = [&](int pi) { return pi >= 1 && pi <= np - 2; };
    auto cornerV = [&](size_t k) -> const Vec3& { return (corners[k].second == 0) ? info[corners[k].first].left : info[corners[k].first].right; };

    struct CornerGroup { size_t c0, c1; bool wall; bool curve; Vec3 V; Vec3 A; };
    vector<CornerGroup> groups;
    groups.reserve(nc);
    for (size_t c = 0; c < nc;)
    {
        size_t c2 = c;
        bool wall = false;
        Vec3 V = pulled[c + 1];
        const int pi = corners[c].first;
        const int side = corners[c].second;
        if (validPortal(pi))
        {
            while (c2 + 1 < nc && validPortal(corners[c2 + 1].first) && corners[c2 + 1].second == side
                && SamePointXZ(cornerV(c2 + 1), cornerV(c)))
                ++c2;
            V = cornerV(c);
            wall = (side == 0) ? info[pi].leftWall : info[pi].rightWall;
        }
        groups.push_back({ c, c2, wall, wall, V, pulled[c + 1] });
        c = c2 + 1;
    }
    const size_t G = groups.size();
    // 코너 상태: 0 = A 를 이등분선 위로 옮겨 곡선, 1 = 원래 Funnel 점 A 에서 곡선 (d 를 반으로 줄이며 V 거리 확인), 2 = Funnel 점 그대로
    vector<uint8> level(G, 0);
    vector<int> owner;     // outPath 각 점을 만든 코너 (-1 = Funnel 점)
    vector<int> srcIdx;    // Funnel 점이면 pulled 번호 (꺾임 비교용), 아니면 -1
    vector<Vec3> pts;

    auto prevCorner = [&](size_t gi) -> Vec3 { return (gi == 0) ? pulled.front() : (groups[gi - 1].curve ? groups[gi - 1].A : pulled[groups[gi - 1].c1 + 1]); };
    auto nextCorner = [&](size_t gi) -> Vec3 { return (gi + 1 == G) ? pulled.back() : (groups[gi + 1].curve ? groups[gi + 1].A : pulled[groups[gi + 1].c0 + 1]); };
    auto turnAt = [](const Vec3& a, const Vec3& b, const Vec3& c) -> float
    {
        float x1 = b.x - a.x, z1 = b.z - a.z, x2 = c.x - b.x, z2 = c.z - b.z;
        const float l1 = sqrtf(x1 * x1 + z1 * z1), l2 = sqrtf(x2 * x2 + z2 * z2);
        if (l1 < 1e-6f || l2 < 1e-6f) return 0.f;
        return acosf(max(-1.f, min(1.f, (x1 * x2 + z1 * z2) / (l1 * l2)))) * 57.29578f;
    };
    // 3차 Bezier 곡선 점 (T1 → T2, 조종점 A1 · A2 쪽 2/3)
    auto bezier = [&](const Vec3& A1, const Vec3& A2, const Vec3& T1, const Vec3& T2, int n)
    {
        const Vec3 P1 = T1 + (A1 - T1) * (2.f / 3.f);
        const Vec3 P2 = T2 + (A2 - T2) * (2.f / 3.f);
        pts.clear();
        pts.push_back(T1);
        for (int i = 1; i < n; ++i)
        {
            const float t = static_cast<float>(i) / n, u = 1.f - t;
            pts.push_back(T1 * (u * u * u) + P1 * (3.f * u * u * t) + P2 * (3.f * u * t * t) + T2 * (t * t * t));
        }
        pts.push_back(T2);
    };

    for (size_t round = 0; round <= 2 * G; ++round)
    {
        for (size_t gi = 0; gi < G; ++gi)
        {
            groups[gi].curve = groups[gi].wall && level[gi] == 0;
            groups[gi].A = pulled[groups[gi].c0 + 1];
        }

        // 1) A' = V - n·R (2번 갱신: 이웃이 옮겨지면 방향이 바뀌므로)
        for (int pass = 0; pass < 2; ++pass)
        {
            for (size_t gi = 0; gi < G; ++gi)
            {
                CornerGroup& g = groups[gi];
                if (!g.curve) continue;
                const Vec3 pp = prevCorner(gi), nn = nextCorner(gi);
                const Vec3 A0 = (pass == 0) ? pulled[g.c0 + 1] : g.A;
                const Vec3 A1 = (pass == 0) ? pulled[g.c1 + 1] : g.A;
                float ux = A0.x - pp.x, uz = A0.z - pp.z, wx = nn.x - A1.x, wz = nn.z - A1.z;
                const float lu = sqrtf(ux * ux + uz * uz), lw = sqrtf(wx * wx + wz * wz);
                if (lu < 1e-5f || lw < 1e-5f) { g.curve = false; g.A = pulled[g.c0 + 1]; continue; }
                ux /= lu; uz /= lu; wx /= lw; wz /= lw;
                float nx = wx - ux, nz = wz - uz;
                const float ln = sqrtf(nx * nx + nz * nz);
                if (ln < 1e-4f) { g.curve = false; g.A = pulled[g.c0 + 1]; continue; }
                nx /= ln; nz /= ln;
                g.A = Vec3(g.V.x - nx * R, pulled[g.c0 + 1].y, g.V.z - nz * R);
            }
        }

        // 2) 곡선 만들기 (판정은 3 에서 한꺼번에)
        outPath.clear(); owner.clear(); srcIdx.clear();
        outPath.push_back(pulled.front()); owner.push_back(-1); srcIdx.push_back(0);
        for (size_t gi = 0; gi < G; ++gi)
        {
            const CornerGroup& g = groups[gi];
            const Vec3 pp = prevCorner(gi), nn = nextCorner(gi);
            const Vec3& last = outPath.back();
            bool emitted = false;
            if (g.curve)
            {
                // 2-a) 옮긴 A' 에서, d 를 각도로 바로 계산
                const Vec3 A = g.A;
                float ux = A.x - pp.x, uz = A.z - pp.z, wx = nn.x - A.x, wz = nn.z - A.z;
                const float lin = sqrtf(ux * ux + uz * uz), lout = sqrtf(wx * wx + wz * wz);
                ux /= lin; uz /= lin; wx /= lout; wz /= lout;
                const float turnDeg = acosf(max(-1.f, min(1.f, ux * wx + uz * wz))) * 57.29578f;
                const float toLast = sqrtf((A.x - last.x) * (A.x - last.x) + (A.z - last.z) * (A.z - last.z));
                const float sinHalf = sinf(turnDeg * 0.5f / 57.29578f);
                float d = FUNNEL_CURVE_DIST;
                d = min(d, lin * ((gi == 0) ? 0.9f : 0.5f));
                d = min(d, lout * ((gi + 1 == G) ? 0.9f : 0.5f));
                d = min(d, toLast * 0.999f);
                if (sinHalf > 1e-4f)
                    d = min(d, 2.f * (R - r) / sinHalf);
                if (turnDeg < FUNNEL_CURVE_MIN_TURN_DEG || d < 0.02f)
                {
                    pts.assign(1, A);
                }
                else
                {
                    const int n = max(2, static_cast<int>(ceilf(turnDeg / FUNNEL_CURVE_STEP_DEG)));
                    bezier(A, A, A + (pp - A) * (d / lin), A + (nn - A) * (d / lout), n);
                }
                emitted = true;
            }
            else if (g.wall && level[gi] == 1)
            {
                // 2-b) 고정 코너: 원래 Funnel 점에서 곡선 (FunnelCurve 방식: V 거리 ≥ r 이 될 때까지 d 를 반으로)
                const Vec3& A1 = pulled[g.c0 + 1];
                const Vec3& A2 = pulled[g.c1 + 1];
                float ux = A1.x - pp.x, uz = A1.z - pp.z, wx = nn.x - A2.x, wz = nn.z - A2.z;
                const float lin = sqrtf(ux * ux + uz * uz), lout = sqrtf(wx * wx + wz * wz);
                if (lin > 1e-5f && lout > 1e-5f)
                {
                    ux /= lin; uz /= lin; wx /= lout; wz /= lout;
                    const float turnDeg = acosf(max(-1.f, min(1.f, ux * wx + uz * wz))) * 57.29578f;
                    const float toLast = sqrtf((A1.x - last.x) * (A1.x - last.x) + (A1.z - last.z) * (A1.z - last.z));
                    float d = min(FUNNEL_CURVE_DIST, min(lin * ((gi == 0) ? 0.9f : 0.5f), min(lout * ((gi + 1 == G) ? 0.9f : 0.5f), toLast * 0.999f)));
                    const int n = max(2, static_cast<int>(ceilf(turnDeg / FUNNEL_CURVE_STEP_DEG)));
                    for (int attempt = 0; attempt < 4 && !emitted && turnDeg >= FUNNEL_CURVE_MIN_TURN_DEG && d >= 0.02f; ++attempt, d *= 0.5f)
                    {
                        bezier(A1, A2, A1 + (pp - A1) * (d / lin), A2 + (nn - A2) * (d / lout), n);
                        bool good = true;
                        for (size_t i = 0; i + 1 < pts.size() && good; ++i)
                            good = CurveSegDistXZ(g.V, pts[i], pts[i + 1]) >= r - 1e-4f;
                        emitted = good;
                    }
                }
            }
            if (emitted)
            {
                for (const Vec3& p : pts) { outPath.push_back(p); owner.push_back(static_cast<int>(gi)); srcIdx.push_back(-1); }
            }
            else
            {
                for (size_t k = g.c0; k <= g.c1; ++k) { outPath.push_back(pulled[k + 1]); owner.push_back(-1); srcIdx.push_back(static_cast<int>(k + 1)); }
            }
        }
        outPath.push_back(pulled.back()); owner.push_back(-1); srcIdx.push_back(static_cast<int>(pulled.size() - 1));

        // 2-c) 도착 판정(0.1) 보다 촘촘한 점은 이동 중 여러 개를 한꺼번에 건너뛰어 방향이 확 바뀐다 → 너무 가까운 점은 뺀다
        {
            size_t w = 1;
            for (size_t i = 1; i < outPath.size(); ++i)
            {
                const float dx = outPath[i].x - outPath[w - 1].x, dz = outPath[i].z - outPath[w - 1].z;
                if (i + 1 < outPath.size() && dx * dx + dz * dz < FUNNEL_CURVE_MIN_SEG * FUNNEL_CURVE_MIN_SEG)
                    continue;
                outPath[w] = outPath[i]; owner[w] = owner[i]; srcIdx[w] = srcIdx[i]; ++w;
            }
            outPath.resize(w); owner.resize(w); srcIdx.resize(w);
        }

        // 3) 모든 선분 통로 판정 + 고정된 Funnel 점에서 원래보다 20° 넘게 더 꺾이면 이웃 코너도 고정
        //    실패한 코너는 한 단계씩 (옮긴 곡선 → 원래 점 곡선 → Funnel 점) 내리고 처음부터 다시
        //    (한쪽만 되돌리면 이웃이 옮긴 점을 기준으로 만든 곡선과 어긋나 경로가 뒤로 꺾인다)
        vector<uint8> bump(G, 0);
        bool anyFix = false;
        int cur = FindTriangleContaining(outPath[0]);
        for (size_t k = 0; k + 1 < outPath.size(); ++k)
        {
            int endTri = -1;
            const bool ok = (cur >= 0) && m_corridorMask[cur] && TraceSegmentInCorridor(outPath[k], cur, outPath[k + 1], m_corridorMask, -1, endTri);
            if (ok) { cur = endTri; continue; }
            for (int o : { owner[k], owner[k + 1] })
                if (o >= 0) { bump[o] = 1; anyFix = true; }
            cur = FindTriangleContaining(outPath[k + 1]);
        }
        if (!anyFix)
        {
            for (size_t k = 1; k + 1 < outPath.size(); ++k)
            {
                const int si = srcIdx[k];
                if (owner[k] != -1 || si <= 0 || si + 1 >= static_cast<int>(pulled.size())) continue;
                if (owner[k - 1] == -1 && owner[k + 1] == -1) continue;
                if (turnAt(outPath[k - 1], outPath[k], outPath[k + 1]) > turnAt(pulled[si - 1], pulled[si], pulled[si + 1]) + FUNNEL_CURVE_KINK_DEG)
                {
                    for (int o : { owner[k - 1], owner[k + 1] })
                        if (o >= 0) { bump[o] = 1; anyFix = true; }
                }
            }
        }
        if (!anyFix)
            break;
        for (size_t gi = 0; gi < G; ++gi)
            if (bump[gi] && level[gi] < 2) ++level[gi];
    }
}

// =============================================================================
// FunnelCurveTangent: 꼭짓점 V 둘레 '안전 원' (반지름 ρ) 에 접하는 직선 + 원호
//  - 이전 코너 → 원의 접점 T1 (접선), T1 → T2 는 V 중심 반지름 ρ 원호, T2 → 다음 코너 (접선)
//  - 접선은 원에 닿기만 하고 원호는 V 에서 정확히 ρ → 그 코너의 V 에 대해 모든 각도에서 ρ 이상 (θ, φ 무관)
//  - 'R = ρ / cos(θ/2)' 를 코너마다 따로 정하는 것과 같다: 완만한 코너는 거의 그대로, 급한 코너만 크게
//  - 이웃 코너는 두 접선의 교점 (A') 을 기준점으로 본다 (3번 갱신)
//  - 검증 · 단계 내림 · 꺾임 검사 · 촘촘한 점 정리는 FunnelCurveAngle 과 같다
//    (0단계 접선 + 원호 → 1단계 원래 Funnel 점에서 Bezier → 2단계 Funnel 점)
// =============================================================================
namespace
{
    constexpr float FUNNEL_TANGENT_RHO_SCALE = 1.2f;    // 안전 원 반지름 ρ = r × 1.2 (r 에 딱 붙지 않게 여유)

    // 부호 있는 반지름 (진행 방향 왼쪽 = +) 두 원 (점 = 0) 의 공통 접선. T1 은 원 1 위, T2 는 원 2 위
    bool CommonTangentXZ(const Vec3& C1, float r1, const Vec3& C2, float r2, Vec3& T1, Vec3& T2)
    {
        const float dx = C2.x - C1.x, dz = C2.z - C1.z;
        const float L = sqrtf(dx * dx + dz * dz);
        if (L < 1e-6f) return false;
        const float dl = r2 - r1;
        if (fabsf(dl) >= L) return false;
        const float hx = dx / L, hz = dz / L;
        const float beta = asinf(dl / L);
        const float cb = cosf(-beta), sb = sinf(-beta);
        const float ux = hx * cb - hz * sb, uz = hx * sb + hz * cb;
        const float nx = -uz, nz = ux;   // 왼쪽 법선
        T1 = Vec3(C1.x - nx * r1, C1.y, C1.z - nz * r1);
        T2 = Vec3(C2.x - nx * r2, C2.y, C2.z - nz * r2);
        return true;
    }
}

void NavMesh::SmoothPathFunnelCurveTangent(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath, vector<pair<Vec3, Vec3>>& outPortals)
{
    outPath.clear();
    const float r = m_agentRadius;
    const float rho = r * FUNNEL_TANGENT_RHO_SCALE;
    vector<NavPortalInfo> info;
    BuildPortals(trianglePath, start, end, outPortals, &info, rho);
    vector<Vec3> pulled;
    vector<pair<int, int>> corners;
    StringPull(outPortals, pulled, &corners);
    if (pulled.size() < 3 || corners.size() + 2 != pulled.size() || info.size() != outPortals.size() || r <= 0.f)
    {
        outPath = pulled;
        return;
    }

    const int np = static_cast<int>(info.size());
    const size_t nc = corners.size();
    auto validPortal = [&](int pi) { return pi >= 1 && pi <= np - 2; };
    auto cornerV = [&](size_t k) -> const Vec3& { return (corners[k].second == 0) ? info[corners[k].first].left : info[corners[k].first].right; };

    struct CornerGroup { size_t c0, c1; bool wall; bool curve; Vec3 V; Vec3 A; Vec3 Tin, Tout; float s; float sweep; };
    vector<CornerGroup> groups;
    groups.reserve(nc);
    for (size_t c = 0; c < nc;)
    {
        size_t c2 = c;
        bool wall = false;
        Vec3 V = pulled[c + 1];
        const int pi = corners[c].first;
        const int side = corners[c].second;
        if (validPortal(pi))
        {
            while (c2 + 1 < nc && validPortal(corners[c2 + 1].first) && corners[c2 + 1].second == side
                && SamePointXZ(cornerV(c2 + 1), cornerV(c)))
                ++c2;
            V = cornerV(c);
            wall = (side == 0) ? info[pi].leftWall : info[pi].rightWall;
        }
        // 도는 방향: 왼쪽으로 꺾이면 원이 진행 방향 왼쪽 (+1)
        const Vec3& A1 = pulled[c + 1];
        const Vec3& A2 = pulled[c2 + 1];
        const Vec3& pv = pulled[c];
        const Vec3& nx = pulled[c2 + 2];
        const float turn = Cross2D(A1.x - pv.x, A1.z - pv.z, nx.x - A2.x, nx.z - A2.z);
        groups.push_back({ c, c2, wall, wall, V, pulled[c + 1], Vec3(), Vec3(), (turn > 0.f) ? 1.f : -1.f, 0.f });
        c = c2 + 1;
    }
    const size_t G = groups.size();
    vector<uint8> level(G, 0);
    vector<int> owner, srcIdx;
    vector<Vec3> pts;

    auto prevCorner = [&](size_t gi) -> Vec3 { return (gi == 0) ? pulled.front() : (groups[gi - 1].curve ? groups[gi - 1].A : pulled[groups[gi - 1].c1 + 1]); };
    auto nextCorner = [&](size_t gi) -> Vec3 { return (gi + 1 == G) ? pulled.back() : (groups[gi + 1].curve ? groups[gi + 1].A : pulled[groups[gi + 1].c0 + 1]); };
    auto turnAt = [](const Vec3& a, const Vec3& b, const Vec3& c) -> float
    {
        float x1 = b.x - a.x, z1 = b.z - a.z, x2 = c.x - b.x, z2 = c.z - b.z;
        const float l1 = sqrtf(x1 * x1 + z1 * z1), l2 = sqrtf(x2 * x2 + z2 * z2);
        if (l1 < 1e-6f || l2 < 1e-6f) return 0.f;
        return acosf(max(-1.f, min(1.f, (x1 * x2 + z1 * z2) / (l1 * l2)))) * 57.29578f;
    };
    auto bezier = [&](const Vec3& A1, const Vec3& A2, const Vec3& T1, const Vec3& T2, int n)
    {
        const Vec3 P1 = T1 + (A1 - T1) * (2.f / 3.f);
        const Vec3 P2 = T2 + (A2 - T2) * (2.f / 3.f);
        pts.clear();
        pts.push_back(T1);
        for (int i = 1; i < n; ++i)
        {
            const float t = static_cast<float>(i) / n, u = 1.f - t;
            pts.push_back(T1 * (u * u * u) + P1 * (3.f * u * u * t) + P2 * (3.f * u * t * t) + T2 * (t * t * t));
        }
        pts.push_back(T2);
    };
    constexpr float TWO_PI = 6.2831853f;

    for (size_t round = 0; round <= 2 * G; ++round)
    {
        for (size_t gi = 0; gi < G; ++gi)
        {
            groups[gi].curve = groups[gi].wall && level[gi] == 0;
            groups[gi].A = pulled[groups[gi].c0 + 1];
        }

        // 1) 접점과 기준점 A' (두 접선의 교점). 이웃이 바뀌면 방향이 바뀌므로 3번 갱신
        vector<uint8> geomFail(G, 0);
        for (int pass = 0; pass < 3; ++pass)
        {
            for (size_t gi = 0; gi < G; ++gi)
            {
                CornerGroup& g = groups[gi];
                if (!g.curve) continue;
                const Vec3 pp = prevCorner(gi), nn = nextCorner(gi);
                const float sr = g.s * rho;
                Vec3 dummy;
                if (!CommonTangentXZ(pp, 0.f, g.V, sr, dummy, g.Tin) || !CommonTangentXZ(g.V, sr, nn, 0.f, g.Tout, dummy))
                {
                    geomFail[gi] = 1; g.curve = false; g.A = pulled[g.c0 + 1];   // 이전 / 다음 코너가 원 안 → 접선 없음
                    continue;
                }
                // 원호 각도 (도는 방향으로 Tin → Tout)
                const float a0 = atan2f(g.Tin.z - g.V.z, g.Tin.x - g.V.x);
                const float a1 = atan2f(g.Tout.z - g.V.z, g.Tout.x - g.V.x);
                float sw = (a1 - a0) * g.s;
                while (sw < 0.f) sw += TWO_PI;
                while (sw >= TWO_PI) sw -= TWO_PI;
                if (sw > 0.75f * TWO_PI)
                {
                    geomFail[gi] = 1; g.curve = false; g.A = pulled[g.c0 + 1];   // 줄이 이 원에 닿지 않음 (반대로 돎)
                    continue;
                }
                g.sweep = sw;
                // 기준점 A' = 두 접선 (pp → Tin, Tout → nn) 의 교점. 거의 평행하면 원호 가운데 바깥 점
                const float d1x = g.Tin.x - pp.x, d1z = g.Tin.z - pp.z, d2x = nn.x - g.Tout.x, d2z = nn.z - g.Tout.z;
                const float den = Cross2D(d1x, d1z, d2x, d2z);
                bool okA = false;
                if (fabsf(den) > 1e-6f)
                {
                    const float t = Cross2D(g.Tout.x - pp.x, g.Tout.z - pp.z, d2x, d2z) / den;
                    const Vec3 X(pp.x + d1x * t, pulled[g.c0 + 1].y, pp.z + d1z * t);
                    const float dv = sqrtf((X.x - g.V.x) * (X.x - g.V.x) + (X.z - g.V.z) * (X.z - g.V.z));
                    if (t > 0.f && dv < 8.f * rho) { g.A = X; okA = true; }
                }
                if (!okA)
                {
                    const float am = a0 + g.s * sw * 0.5f;
                    g.A = Vec3(g.V.x + cosf(am) * rho, pulled[g.c0 + 1].y, g.V.z + sinf(am) * rho);
                }
            }
        }

        // 2) 경로 만들기
        outPath.clear(); owner.clear(); srcIdx.clear();
        outPath.push_back(pulled.front()); owner.push_back(-1); srcIdx.push_back(0);
        for (size_t gi = 0; gi < G; ++gi)
        {
            const CornerGroup& g = groups[gi];
            const Vec3 pp = prevCorner(gi), nn = nextCorner(gi);
            const Vec3& last = outPath.back();
            bool emitted = false;
            if (g.curve)
            {
                // 2-a) 접점 Tin → 원호 → 접점 Tout
                const float a0 = atan2f(g.Tin.z - g.V.z, g.Tin.x - g.V.x);
                const int n = max(1, static_cast<int>(ceilf(g.sweep * 57.29578f / FUNNEL_CURVE_STEP_DEG)));
                pts.clear();
                pts.push_back(Vec3(g.Tin.x, g.A.y, g.Tin.z));
                for (int i = 1; i < n; ++i)
                {
                    const float a = a0 + g.s * g.sweep * static_cast<float>(i) / n;
                    pts.push_back(Vec3(g.V.x + cosf(a) * rho, g.A.y, g.V.z + sinf(a) * rho));
                }
                pts.push_back(Vec3(g.Tout.x, g.A.y, g.Tout.z));
                emitted = true;
            }
            else if (g.wall && level[gi] == 1)
            {
                // 2-b) 원래 Funnel 점에서 Bezier (FunnelCurve 방식: V 거리 ≥ r 이 될 때까지 d 를 반으로)
                const Vec3& A1 = pulled[g.c0 + 1];
                const Vec3& A2 = pulled[g.c1 + 1];
                float ux = A1.x - pp.x, uz = A1.z - pp.z, wx = nn.x - A2.x, wz = nn.z - A2.z;
                const float lin = sqrtf(ux * ux + uz * uz), lout = sqrtf(wx * wx + wz * wz);
                if (lin > 1e-5f && lout > 1e-5f)
                {
                    ux /= lin; uz /= lin; wx /= lout; wz /= lout;
                    const float turnDeg = acosf(max(-1.f, min(1.f, ux * wx + uz * wz))) * 57.29578f;
                    const float toLast = sqrtf((A1.x - last.x) * (A1.x - last.x) + (A1.z - last.z) * (A1.z - last.z));
                    float d = min(FUNNEL_CURVE_DIST, min(lin * ((gi == 0) ? 0.9f : 0.5f), min(lout * ((gi + 1 == G) ? 0.9f : 0.5f), toLast * 0.999f)));
                    const int n = max(2, static_cast<int>(ceilf(turnDeg / FUNNEL_CURVE_STEP_DEG)));
                    for (int attempt = 0; attempt < 4 && !emitted && turnDeg >= FUNNEL_CURVE_MIN_TURN_DEG && d >= 0.02f; ++attempt, d *= 0.5f)
                    {
                        bezier(A1, A2, A1 + (pp - A1) * (d / lin), A2 + (nn - A2) * (d / lout), n);
                        bool good = true;
                        for (size_t i = 0; i + 1 < pts.size() && good; ++i)
                            good = CurveSegDistXZ(g.V, pts[i], pts[i + 1]) >= r - 1e-4f;
                        emitted = good;
                    }
                }
            }
            if (emitted)
            {
                for (const Vec3& p : pts) { outPath.push_back(p); owner.push_back(static_cast<int>(gi)); srcIdx.push_back(-1); }
            }
            else
            {
                for (size_t k = g.c0; k <= g.c1; ++k) { outPath.push_back(pulled[k + 1]); owner.push_back(-1); srcIdx.push_back(static_cast<int>(k + 1)); }
            }
        }
        outPath.push_back(pulled.back()); owner.push_back(-1); srcIdx.push_back(static_cast<int>(pulled.size() - 1));

        // 2-c) 너무 가까운 점 정리 (도착 판정 0.1 보다 촘촘하면 이동 방향이 튄다)
        {
            size_t w = 1;
            for (size_t i = 1; i < outPath.size(); ++i)
            {
                const float dx = outPath[i].x - outPath[w - 1].x, dz = outPath[i].z - outPath[w - 1].z;
                if (i + 1 < outPath.size() && dx * dx + dz * dz < FUNNEL_CURVE_MIN_SEG * FUNNEL_CURVE_MIN_SEG)
                    continue;
                outPath[w] = outPath[i]; owner[w] = owner[i]; srcIdx[w] = srcIdx[i]; ++w;
            }
            outPath.resize(w); owner.resize(w); srcIdx.resize(w);
        }

        // 3) 모든 선분 통로 판정 + 고정 점 꺾임 검사 → 실패한 코너는 한 단계 내리고 처음부터
        vector<uint8> bump(G, 0);
        bool anyFix = false;
        for (size_t gi = 0; gi < G; ++gi)
            if (geomFail[gi] && level[gi] == 0) { bump[gi] = 1; anyFix = true; }
        int cur = FindTriangleContaining(outPath[0]);
        for (size_t k = 0; k + 1 < outPath.size(); ++k)
        {
            int endTri = -1;
            const bool ok = (cur >= 0) && m_corridorMask[cur] && TraceSegmentInCorridor(outPath[k], cur, outPath[k + 1], m_corridorMask, -1, endTri);
            if (ok) { cur = endTri; continue; }
            for (int o : { owner[k], owner[k + 1] })
                if (o >= 0) { bump[o] = 1; anyFix = true; }
            cur = FindTriangleContaining(outPath[k + 1]);
        }
        if (!anyFix)
        {
            for (size_t k = 1; k + 1 < outPath.size(); ++k)
            {
                const int si = srcIdx[k];
                if (owner[k] != -1 || si <= 0 || si + 1 >= static_cast<int>(pulled.size())) continue;
                if (owner[k - 1] == -1 && owner[k + 1] == -1) continue;
                if (turnAt(outPath[k - 1], outPath[k], outPath[k + 1]) > turnAt(pulled[si - 1], pulled[si], pulled[si + 1]) + FUNNEL_CURVE_KINK_DEG)
                {
                    for (int o : { owner[k - 1], owner[k + 1] })
                        if (o >= 0) { bump[o] = 1; anyFix = true; }
                }
            }
        }
        if (!anyFix)
            break;
        for (size_t gi = 0; gi < G; ++gi)
            if (bump[gi] && level[gi] < 2) ++level[gi];
    }
}

// =============================================================================
// F3: 이동 중 위치 보정
//  기존 GetNearestPointOnNavMesh 는 삼각형 밖의 점을 "가장 가까운 꼭짓점"으로 보내서
//  벽에 조금만 닿아도 크게 튕긴다. 여기서는 "삼각형 위의 최근접점"(변 위로 미끄러짐)을 쓴다.
// =============================================================================
namespace
{
    // Ericson, Real-Time Collision Detection 5.1.5 - 점 p 에서 삼각형 abc 위 최근접점
    Vec3 ClosestPointOnTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c)
    {
        const Vec3 ab = b - a, ac = c - a, ap = p - a;
        const float d1 = ab.Dot(ap), d2 = ac.Dot(ap);
        if (d1 <= 0.f && d2 <= 0.f) return a;

        const Vec3 bp = p - b;
        const float d3 = ab.Dot(bp), d4 = ac.Dot(bp);
        if (d3 >= 0.f && d4 <= d3) return b;

        const float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
            return a + ab * (d1 / (d1 - d3));

        const Vec3 cp = p - c;
        const float d5 = ab.Dot(cp), d6 = ac.Dot(cp);
        if (d6 >= 0.f && d5 <= d6) return c;

        const float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
            return a + ac * (d2 / (d2 - d6));

        const float va = d3 * d6 - d5 * d4;
        if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
            return b + (c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6)));

        const float denom = 1.f / (va + vb + vc);
        return a + ab * (vb * denom) + ac * (vc * denom);
    }
}

Vec3 NavMesh::GetClosestPointOnNavMesh(const Vec3& worldPos)
{
    if (m_triangles.empty())
        return worldPos;

    // 삼각형 안이면 높이만 그 삼각형 평면에 맞춘다 (수평 이동은 그대로)
    const int inside = FindTriangleContaining(worldPos);
    if (inside != -1)
    {
        const NavMeshTriangle& tri = m_triangles[inside];
        Vec3 onPlane = worldPos;
        onPlane.y = HeightOnTriangle(tri, worldPos.x, worldPos.z, worldPos.y);   // 수직으로 높이만
        return onPlane;
    }

    // 밖이면 주변 3x3 셀의 삼각형 위 최근접점 중 가장 가까운 점
    const int cx = m_spatialGrid.CellIndex(worldPos.x);
    const int cz = m_spatialGrid.CellIndex(worldPos.z);
    Vec3 best = worldPos;
    float bestDistSq = FLT_MAX;
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dz = -1; dz <= 1; ++dz)
        {
            auto it = m_spatialGrid.grid.find(m_spatialGrid.MakeKey(cx + dx, cz + dz));
            if (it == m_spatialGrid.grid.end())
                continue;
            for (int idx : it->second)
            {
                // XZ 평면에서 최근접점을 구하고 높이는 그 삼각형 평면에서 계산 (높이 차로 엉뚱한 삼각형을 고르지 않게)
                const NavMeshTriangle& tri = m_triangles[idx];
                const Vec3 p2(worldPos.x, 0.f, worldPos.z);
                const Vec3 a2(tri.vertices[0].x, 0.f, tri.vertices[0].z);
                const Vec3 b2(tri.vertices[1].x, 0.f, tri.vertices[1].z);
                const Vec3 c2(tri.vertices[2].x, 0.f, tri.vertices[2].z);
                const Vec3 q2 = ClosestPointOnTriangle(p2, a2, b2, c2);
                const float distSq = (q2.x - p2.x) * (q2.x - p2.x) + (q2.z - p2.z) * (q2.z - p2.z);
                if (distSq < bestDistSq)
                {
                    bestDistSq = distSq;
                    best = Vec3(q2.x, HeightOnTriangle(tri, q2.x, q2.z, worldPos.y), q2.z);
                }
            }
        }
    }

    if (bestDistSq == FLT_MAX)
        return GetNearestPointOnNavMesh(worldPos);   // 주변에 삼각형이 없으면 기존 방식
    return best;
}

Vec3 NavMesh::CorrectAgentPosition(const Vec3& worldPos)
{
    return m_edgeSnap ? GetClosestPointOnNavMesh(worldPos) : GetNearestPointOnNavMesh(worldPos);
}
