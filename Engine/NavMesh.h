#pragma once
#include "Component.h"

struct NavMeshTriangle {
    Vec3 vertices[3];
    Vec3 normal;
    Vec3 center;
    vector<int> neighbors;  // 인접한 삼각형들의 인덱스
    int index;              // 자신의 인덱스
};

struct PathNode {
    int triangleIndex;
    Vec3 position;
    float gCost;    // 시작점으로부터의 실제 거리
    float hCost;    // 목표점까지의 휴리스틱 거리
    float fCost;    // gCost + hCost
    int parent;     // 부모 노드

    PathNode() : triangleIndex(-1), gCost(0), hCost(0), fCost(0), parent(-1) {}
};

class NavMesh : public Component
{
    using Super = Component;

public:
    NavMesh();
    virtual ~NavMesh();

    virtual void Start() override;
    virtual void Update() override;

    // NavMesh 데이터 로드
    void LoadNavMeshData();

    // 월드 좌표를 NavMesh 상의 가장 가까운 점으로 변환
    Vec3 GetNearestPointOnNavMesh(const Vec3& worldPos);

    // 두 점 사이의 경로 찾기
    vector<Vec3> FindPath(const Vec3& start, const Vec3& end);

    // 해당 위치가 NavMesh 위에 있는지 확인
    bool IsOnNavMesh(const Vec3& worldPos, float tolerance = 1.0f);

    // Ray와 NavMesh의 교차점 찾기
    bool RaycastNavMesh(const Ray& ray, Vec3& hitPoint);

    void DebugPrintTriangles();

    void BuildTriangleConnections();
    bool AreTrianglesAdjacent(const NavMeshTriangle& tri1, const NavMeshTriangle& tri2);
    int getTotalNeighbors();

    vector<int> FindTrianglePath(int startTriangle, int endTriangle);
    vector<int> ReconstructPath(const vector<PathNode>& allNodes, int endTriangle);
    int FindTriangleContaining(const Vec3& point);
    vector<Vec3> ConvertTrianglePathToWorldPath(const vector<int>& trianglePath,
        const Vec3& start, const Vec3& end);

    // 경로 스무딩 관련 함수들
    vector<Vec3> SmoothPath(const vector<Vec3>& originalPath);
    bool HasLineOfSight(const Vec3& start, const Vec3& end);
    bool IsLineOnNavMesh(const Vec3& start, const Vec3& end, float stepSize = 0.5f);

private:
    vector<NavMeshTriangle> m_triangles;
    shared_ptr<Model> m_navMeshModel;

    // A* 경로 찾기를 위한 내부 함수들
    float GetDistance(const Vec3& a, const Vec3& b);
    Vec3 ProjectPointOnTriangle(const Vec3& point, const NavMeshTriangle& triangle);
    bool IsPointInTriangle(const Vec3& point, const NavMeshTriangle& triangle);



};
