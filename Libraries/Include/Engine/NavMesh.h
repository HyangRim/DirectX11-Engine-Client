#pragma once
#include "Component.h"

struct NavTriangle {
    Vec3 vertices[3];
    Vec3 center;
    Vec3 normal;
    vector<int32> neighbors; // 인접한 삼각형 인덱스들
    bool isWalkable = true;
    int32 id;
    float cost = 1.0f; // 이동 비용
};

struct NavEdge {
    Vec3 start;
    Vec3 end;
    Vec3 center;
    float length;
    int32 triangleA = -1;
    int32 triangleB = -1;
};

struct NavNode {
    Vec3 position;
    float gCost = 0.0f;  // 시작점으로부터의 실제 거리
    float hCost = 0.0f;  // 목표점까지의 추정 거리
    float fCost = 0.0f;  // g + h
    int32 triangleIndex = -1;
    int32 parentIndex = -1;
    bool inOpenSet = false;
    bool inClosedSet = false;
};

class NavMesh : public Component
{
public:
    NavMesh();
    virtual ~NavMesh();

    virtual void Init() override;
    virtual void Update() override;
    virtual void Start() override;

    // NavMesh 생성 및 관리
    void GenerateNavMesh(shared_ptr<GameObject> walkableArea, const vector<shared_ptr<GameObject>>& obstacles);
    void BuildTriangulation();
    void FindNeighbors();
    void OptimizeMesh();

    // 경로 탐색
    vector<Vec3> FindPath(const Vec3& start, const Vec3& end);
    bool IsPointOnNavMesh(const Vec3& point);
    Vec3 GetNearestPointOnNavMesh(const Vec3& point);
    int32 GetTriangleContainingPoint(const Vec3& point);

    // 시각화
    void CreateDebugVisualization();
    void SetDebugVisible(bool visible) { m_debugVisible = visible; }
    bool IsDebugVisible() const { return m_debugVisible; }

    // Getter
    const vector<NavTriangle>& GetTriangles() const { return m_triangles; }
    const vector<NavEdge>& GetEdges() const { return m_edges; }

private:
    // 내부 구현 함수들
    bool IsPointInTriangle(const Vec3& point, const NavTriangle& triangle);
    float CalculateTriangleArea(const Vec3& a, const Vec3& b, const Vec3& c);
    bool LineIntersectsObstacle(const Vec3& start, const Vec3& end, const vector<shared_ptr<GameObject>>& obstacles);
    void SimplifyPath(vector<Vec3>& path);

    // A* 알고리즘 관련
    vector<int32> AStar(int32 startTriangle, int32 endTriangle);
    float GetDistance(const Vec3& a, const Vec3& b);
    float GetHeuristic(const Vec3& a, const Vec3& b);

    // 삼각분할 관련
    void DelaunayTriangulation(const vector<Vec3>& points);
    bool IsValidTriangle(const NavTriangle& triangle, const vector<shared_ptr<GameObject>>& obstacles);

private:
    vector<NavTriangle> m_triangles;
    vector<NavEdge> m_edges;
    vector<Vec3> m_vertices;

    // 디버그 시각화
    bool m_debugVisible = true;
    shared_ptr<GameObject> m_debugObject;
    vector<shared_ptr<GameObject>> m_debugTriangleObjects;

    // 경로 탐색 캐시
    vector<NavNode> m_nodes;

    // NavMesh 설정
    float m_triangleMaxSize = 5.0f;
    float m_agentRadius = 1.0f;
    BoundingBox m_bounds;
};
