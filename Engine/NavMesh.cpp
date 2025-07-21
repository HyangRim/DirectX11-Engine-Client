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
        float distance = GetDistance(worldPos, projectedPoint);

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

vector<Vec3> NavMesh::FindPath(const Vec3& start, const Vec3& end)
{
    vector<Vec3> path;

    int startTriangle = FindTriangleContaining(start);
    int endTriangle = FindTriangleContaining(end);

    // NavMesh 위에 없는 점들을 가장 가까운 점으로 보정
    if (startTriangle == -1 || endTriangle == -1)
    {
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
        path.push_back(GetNearestPointOnNavMesh(start));
        path.push_back(GetNearestPointOnNavMesh(end));
        return path;
    }

    // 삼각형 경로를 월드 좌표 경로로 변환 후 스무딩
    vector<Vec3> rawPath = ConvertTrianglePathToWorldPath(trianglePath, start, end);
    path = SmoothPath(rawPath);

    return path;
}

vector<int> NavMesh::FindTrianglePath(int startTriangle, int endTriangle)
{
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
            return ReconstructPath(allNodes, endTriangle);
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

vector<Vec3> NavMesh::ConvertTrianglePathToWorldPath(const vector<int>& trianglePath, const Vec3& start, const Vec3& end)
{
    vector<Vec3> worldPath;
    if (trianglePath.empty()) return worldPath;

    worldPath.push_back(start);

    for (size_t i = 1; i < trianglePath.size() - 1; i++)
    {
        worldPath.push_back(m_triangles[trianglePath[i]].center);
    }

    worldPath.push_back(end);
    return worldPath;
}

vector<Vec3> NavMesh::SmoothPath(const vector<Vec3>& originalPath)
{
    if (originalPath.size() <= 2) return originalPath;

    vector<Vec3> smoothedPath;
    smoothedPath.push_back(originalPath[0]);

    size_t currentIndex = 0;
    while (currentIndex < originalPath.size() - 1)
    {
        size_t farthestIndex = currentIndex + 1;

        // 직선으로 갈 수 있는 가장 먼 지점 찾기
        for (size_t i = currentIndex + 2; i < originalPath.size(); i++)
        {
            if (HasLineOfSight(smoothedPath.back(), originalPath[i]))
            {
                farthestIndex = i;
            }
            else
            {
                break;
            }
        }

        if (farthestIndex < originalPath.size() - 1)
        {
            smoothedPath.push_back(originalPath[farthestIndex]);
        }

        currentIndex = farthestIndex;
    }

    smoothedPath.push_back(originalPath.back());
    return smoothedPath;
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
    for (size_t i = 0; i < m_triangles.size(); i++)
    {
        if (IsPointInTriangle(point, m_triangles[i]))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

Vec3 NavMesh::ProjectPointOnTriangle(const Vec3& point, const NavMeshTriangle& triangle)
{
    Vec3 v0 = triangle.vertices[0];
    Vec3 planePoint = point - (point - v0).Dot(triangle.normal) * triangle.normal;

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

bool NavMesh::HasLineOfSight(const Vec3& start, const Vec3& end)
{
    return GetDistance(start, end) < 0.1f || IsLineOnNavMesh(start, end);
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
