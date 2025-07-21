#pragma once
#include "Component.h"
#include <queue>

// 삼각형 구조체
struct NavMeshTriangle {
    Vec3 vertices[3];
    Vec3 normal;
    Vec3 center;
    vector<int> neighbors;
    int index;

    NavMeshTriangle() : index(-1) {}
};

// A* 경로찾기용 노드
struct PathNode {
    int triangleIndex;
    Vec3 position;
    float gCost, hCost, fCost;
    int parent;

    PathNode() : triangleIndex(-1), gCost(0), hCost(0), fCost(0), parent(-1) {}

    bool operator<(const PathNode& other) const {
        return fCost > other.fCost; // priority_queue를 위한 비교연산자
    }
};

class NavMesh : public Component
{
    using Super = Component;

public:
    NavMesh();
    virtual ~NavMesh();

    virtual void Start() override;
    virtual void Update() override;

    // 핵심 기능
    void LoadNavMeshData();
    Vec3 GetNearestPointOnNavMesh(const Vec3& worldPos);
    vector<Vec3> FindPath(const Vec3& start, const Vec3& end);
    bool IsOnNavMesh(const Vec3& worldPos, float tolerance = 1.0f);
    bool RaycastNavMesh(const Ray& ray, Vec3& hitPoint);

    // 디버그 기능
    void DebugPrintTriangles();

private:
    // 멤버 변수
    vector<NavMeshTriangle> m_triangles;
    shared_ptr<Model> m_navMeshModel;

    // 초기화 및 전처리
    void BuildTriangleConnections();
    bool AreTrianglesAdjacent(const NavMeshTriangle& tri1, const NavMeshTriangle& tri2);

    // A* 경로찾기
    vector<int> FindTrianglePath(int startTriangle, int endTriangle);
    vector<int> ReconstructPath(const vector<PathNode>& allNodes, int endTriangle);

    // 경로 변환 및 최적화
    vector<Vec3> ConvertTrianglePathToWorldPath(const vector<int>& trianglePath, const Vec3& start, const Vec3& end);
    vector<Vec3> SmoothPath(const vector<Vec3>& originalPath);

    // 유틸리티 함수
    int FindTriangleContaining(const Vec3& point);
    Vec3 ProjectPointOnTriangle(const Vec3& point, const NavMeshTriangle& triangle);
    bool IsPointInTriangle(const Vec3& point, const NavMeshTriangle& triangle);
    bool HasLineOfSight(const Vec3& start, const Vec3& end);
    bool IsLineOnNavMesh(const Vec3& start, const Vec3& end, float stepSize = 0.5f);
    float GetDistance(const Vec3& a, const Vec3& b);
    int GetTotalNeighbors() const;
};
