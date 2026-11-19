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

    // 변 단위 이웃: edgeNeighbor[i] = 변 (vertices[i], vertices[(i+1)%3]) 건너편 삼각형 (-1 = 경계/벽)
    int edgeNeighbor[3] = { -1, -1, -1 };
    // 꼭짓점이 NavMesh 경계(벽) 위에 있는가. Funnel에서 벽 쪽 Portal 끝만 Agent 반경만큼 줄이는 데 사용
    bool boundaryVertex[3] = { false, false, false };

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
        return fCost > other.fCost;
    }
};

//Grid 기반 공간 분할
struct SpatialGrid {
    float cellsize = 25.f;
    unordered_map<uint64, vector<int>> grid;

    // 셀 좌표: floor 로 자른다 (int 캐스트는 0 쪽으로 잘려 (-cellsize, cellsize) 구간이 셀 0 하나로 합쳐짐)
    int CellIndex(float v) const { return static_cast<int>(floorf(v / cellsize)); }

    // (x, z) 셀 → 키. uint32 를 거쳐 변환해야 음수 z 의 부호 확장이 상위 32비트(x)를 덮어쓰지 않는다
    uint64 MakeKey(int x, int z) const {
        return (static_cast<uint64>(static_cast<uint32>(x)) << 32) | static_cast<uint64>(static_cast<uint32>(z));
    }

    uint64 GetKey(const Vec3& _pos) const {
        return MakeKey(CellIndex(_pos.x), CellIndex(_pos.z));
    }

    void AddTriangle(int idx, const NavMeshTriangle& _tri) {
        Vec3 minBound = GetMinBound(_tri);
        Vec3 maxBound = GetMaxBound(_tri);

        int minX = CellIndex(minBound.x);
        int maxX = CellIndex(maxBound.x);
        int minZ = CellIndex(minBound.z);
        int maxZ = CellIndex(maxBound.z);

        for (int x = minX; x <= maxX; x++) {
            for (int z = minZ; z <= maxZ; z++) {
                grid[MakeKey(x, z)].emplace_back(idx);
            }
        }
    }

    Vec3 GetMinBound(const NavMeshTriangle& tri) const {
        const Vec3& v0 = tri.vertices[0];
        const Vec3& v1 = tri.vertices[1];
        const Vec3& v2 = tri.vertices[2];
        return Vec3(
            min(min(v0.x, v1.x), v2.x),
            min(min(v0.y, v1.y), v2.y),
            min(min(v0.z, v1.z), v2.z)
        );
    }

    // 주어진 삼각형의 세 정점으로부터 AABB 최대점 반환
    Vec3 GetMaxBound(const NavMeshTriangle& tri) const {
        const Vec3& v0 = tri.vertices[0];
        const Vec3& v1 = tri.vertices[1];
        const Vec3& v2 = tri.vertices[2];
        return Vec3(
            max(max(v0.x, v1.x), v2.x),
            max(max(v0.y, v1.y), v2.y),
            max(max(v0.z, v1.z), v2.z)
        );
    }
};

// =============================================================================
// 경로 진단 (Diagnostics)
// FindPath가 어떤 분기로 끝났는지와 스무딩 전/후 경로를 기록한다.
// =============================================================================
// 스무딩 방식 (F7로 순환)
enum class NavSmoothingMode
{
    LOS,        // 기존: 중심점 경로 + LOS 점 샘플링 (오차 허용 1.0)
    Corridor,   // 중심점 경로 + 통로 삼각형 따라 걷기 판정
    CorridorPull, // Corridor + 꺾이는 점을 코너 벽 꼭짓점 쪽으로 당김 (반경 m_agentRadius 만큼 띄움) - 보류
    CorridorDP,   // 같은 중점 후보 + 통로 판정, '가장 먼 점' 대신 '총 길이 최소 조합'(DP) 선택
    CorridorSplit, // 가이드 분할: 중점 경로를 가이드로, 막히는 직선을 벽 꼭짓점에서 재귀적으로 쪼갬 (반경 m_agentRadius 여유)
    CorridorFan,   // 가이드 분할 + 꺾는 점을 코너 꼭짓점에 붙은 내부 변 위(V 에서 r)에 바로 배치 (마무리 반복 없음)
    CorridorFanPortal, // CorridorFan 과 같은 분할 + 부채꼴 점, 통로/여유/꼭짓점 판정을 Portal 순서로 (O(V·n) → O(n))
    Funnel,     // Portal 목록으로 String Pulling (코너 꼭짓점에서 꺾는 최단 경로)
    FunnelFan,  // Funnel 로 코너 꼭짓점을 찾고, 코너는 부채꼴 점(V 에서 r, 붙은 Portal 마다)으로 나눠 돈다
    FunnelCurve, // Funnel 코너를 3차 Bezier 곡선으로 (반경 1.5r 로 축소 후 곡선, V 에서 r 이상 + 통로 판정으로 검증)
    FunnelCurveAngle, // FunnelCurve + 코너 기준점을 V 의 이등분선 위로 (φ = 0), d 를 각도로 바로 계산 (d ≤ 2(R - r)/sin(θ/2))
    FunnelCurveTangent, // 꼭짓점 V 둘레 안전 원 (반지름 1.2r) 의 접선 + 원호: 모든 각도에서 V 로부터 원 반지름 이상
};

enum class NavPathBranch
{
    None,
    SameTriangle,       // 시작/도착이 같은 삼각형 → 직선
    TriangleNotFound,   // 시작 또는 도착 삼각형을 못 찾음 → A* 없이 직선 대체 (관통 위험)
    AStarFailed,        // A* 실패 → 직선 대체 (관통 위험)
    AStarSmoothed,      // 정상: A* + 스무딩
};

struct NavPathDebugInfo
{
    NavPathBranch branch = NavPathBranch::None;
    Vec3 start;
    Vec3 end;
    int startTriangle = -1;
    int endTriangle = -1;
    // 삼각형을 못 찾았을 때만 채움: 전체 삼각형을 오차 허용으로 다시 검사한 결과
    // (-1이면 실제로 NavMesh 밖, 아니면 Grid/경계 판정 문제)
    int bruteStartTriangle = -1;
    int bruteEndTriangle = -1;
    vector<Vec3> rawPath;   // A* 중심점 경로 (스무딩 전)
    vector<Vec3> finalPath; // 최종 경로
    NavSmoothingMode smoothingMode = NavSmoothingMode::LOS;
    vector<pair<Vec3, Vec3>> portals; // Funnel 입력 (left, right). 처음 = (S,S), 마지막 = (G,G)
};

// Portal 원본 정보 (반경 축소 전). FunnelFan 이 코너 꼭짓점과 그 둘레 Portal 을 찾는 데 쓴다
struct NavPortalInfo
{
    Vec3 left, right;                    // 축소 전 끝점 (BuildPortals 의 left / right 와 같은 방향)
    bool leftWall = false, rightWall = false;
    int tri = -1;                        // Portal 을 넘어간 쪽 삼각형 (처음 = 출발 삼각형, 끝 = 도착 삼각형)
};

class NavMesh : public Component
{
    using Super = Component;

public:
    SpatialGrid m_spatialGrid;

    NavMesh();
    virtual ~NavMesh();

    virtual void Start() override;
    virtual void Update() override;

    // 핵심 기능 - 성능 최적화된 참조 버전
    void LoadNavMeshData();
    Vec3 GetNearestPointOnNavMesh(const Vec3& worldPos);
    void FindPath(const Vec3& start, const Vec3& end, vector<Vec3>& outPath); // 참조로 변경
    bool IsOnNavMesh(const Vec3& worldPos, float tolerance = 1.0f);
    bool RaycastNavMesh(const Ray& ray, Vec3& hitPoint);

    // 디버그 기능
    void DebugPrintTriangles();

    // 스무딩 방식 선택 (LOS / Corridor / Funnel)
    void SetSmoothingMode(NavSmoothingMode mode) { m_smoothingMode = mode; }
    NavSmoothingMode GetSmoothingMode() const { return m_smoothingMode; }
    static const char* SmoothingModeName(NavSmoothingMode mode);

    // F1: Agent 반경. Funnel Portal 의 벽 쪽 끝을 이만큼 안쪽으로 줄여 코너와 거리를 둔다 (0 = 끔)
    void SetAgentRadius(float radius) { m_agentRadius = max(0.f, radius); }
    float GetAgentRadius() const { return m_agentRadius; }

    // F3: 이동 중 위치 보정 방식. true = 삼각형 위 최근접점(변으로 미끄러짐), false = 기존(꼭짓점 스냅)
    void SetEdgeSnap(bool enabled) { m_edgeSnap = enabled; }
    bool IsEdgeSnap() const { return m_edgeSnap; }
    Vec3 CorrectAgentPosition(const Vec3& worldPos);
    Vec3 GetClosestPointOnNavMesh(const Vec3& worldPos);

    // 선분 from→to 를 통로 안에서 삼각형 따라 걸으며 추적. 성공하면 to 가 든 삼각형을 outEndTri 로 돌려준다.
    // stopTri 가 주어지면(>=0) 그 삼각형에 닿는 즉시 성공
    bool TraceSegmentInCorridor(const Vec3& from, int fromTri, const Vec3& to, const vector<uint8>& inCorridor, int stopTri, int& outEndTri) const;

    // 선분 from→to 가 NavMesh 위에만 있는지 (통로 제한 없이 삼각형 따라 걷기) - Look-ahead 추종 검증용
    bool IsSegmentOnNavMesh(const Vec3& from, const Vec3& to);

    // 선분 from→to 가 A* 통로 삼각형들만 지나는지 (삼각형을 따라 걸으며 정확히 판정)
    bool IsSegmentInCorridor(const Vec3& from, int fromTri, const Vec3& to, int toTri, const vector<uint8>& inCorridor) const;

    // 경로 진단
    const NavPathDebugInfo& GetLastPathDebug() const { return m_lastPathDebug; }
    // 마지막 FindPath 구간별 시간 (us): A* + 원시 경로 변환 / 스무딩만 (A* 분기가 아니면 0)
    double GetLastAStarUs() const { return m_lastAStarUs; }
    double GetLastSmoothUs() const { return m_lastSmoothUs; }
    bool IsPointInsideNavMesh(const Vec3& worldPos) { return FindTriangleContaining(worldPos) != -1; }
    int FindTriangleBruteForce(const Vec3& worldPos, float epsilon = 1e-4f) const;
    static const char* BranchName(NavPathBranch branch);

    // 벤치마크용 읽기 전용 접근
    int GetTriangleCount() const { return static_cast<int>(m_triangles.size()); }
    const NavMeshTriangle& GetTriangle(int index) const { return m_triangles[index]; }

private:
    // 멤버 변수
    vector<NavMeshTriangle> m_triangles;
    shared_ptr<Model> m_navMeshModel;

    mutable int m_lastFoundTriangle = -1;

    // 마지막 FindPath 진단 정보
    NavPathDebugInfo m_lastPathDebug;
    double m_lastAStarUs = 0.0;
    double m_lastSmoothUs = 0.0;

    // 통로 기반 스무딩
    NavSmoothingMode m_smoothingMode = NavSmoothingMode::Corridor;   // 기본: Corridor (LOS 보완)
    float m_agentRadius = 0.3f;
    bool m_edgeSnap = true;
    vector<uint8> m_corridorMask;   // 삼각형 수만큼, A* 통로에 속하면 1 (요청마다 표시 후 원복)
    vector<uint8> m_allMask;        // 전부 1 (통로 제한 없는 판정용)
    void BuildEdgeNeighbors();
    void SmoothPathCorridor(const vector<Vec3>& rawPath, const vector<int>& trianglePath, vector<Vec3>& outPath, vector<int>& outTris);
    // Corridor 결과의 중간 점을 코너 벽 꼭짓점에서 반경만큼 떨어진 곳으로 당긴다 (통로 판정으로 검증, 실패 시 덜 당김)
    void SmoothPathCorridorSplit(const vector<Vec3>& rawPath, const vector<int>& trianglePath, vector<Vec3>& outPath, vector<int>& outTris, bool fanPoints = false);
    void SmoothPathCorridorDP(const vector<Vec3>& rawPath, const vector<int>& trianglePath, vector<Vec3>& outPath, vector<int>& outTris);
    void SmoothPathCorridorFanPortal(const vector<Vec3>& rawPath, const vector<int>& trianglePath, vector<Vec3>& outPath, vector<int>& outTris);
    void PullCornersToWall(const vector<int>& trianglePath, vector<Vec3>& path, vector<int>& pathTris) const;
    int FindCornerBoundaryVertex(const vector<int>& trianglePath, const Vec3& prev, const Vec3& cur, const Vec3& next, Vec3& outVertex) const;

    // Funnel
    void BuildPortals(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<pair<Vec3, Vec3>>& outPortals, vector<NavPortalInfo>* outInfo = nullptr, float shrinkRadius = -1.f) const;   // shrinkRadius < 0 이면 m_agentRadius
    // outCorners: 꺾은 점마다 (Portal 번호, 0 = left / 1 = right). outPath[1 .. size-2] 와 같은 순서
    void StringPull(const vector<pair<Vec3, Vec3>>& portals, vector<Vec3>& outPath, vector<pair<int, int>>* outCorners = nullptr) const;
    void SmoothPathFunnelFan(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath, vector<pair<Vec3, Vec3>>& outPortals);
    void SmoothPathFunnelCurve(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath, vector<pair<Vec3, Vec3>>& outPortals);
    void SmoothPathFunnelCurveAngle(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath, vector<pair<Vec3, Vec3>>& outPortals);
    void SmoothPathFunnelCurveTangent(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath, vector<pair<Vec3, Vec3>>& outPortals);

    // 초기화 및 전처리
    void BuildTriangleConnections();
    void InitializeSpatialGrid();
    bool AreTrianglesAdjacent(const NavMeshTriangle& tri1, const NavMeshTriangle& tri2);

    // A* 경로찾기 - 성능 최적화된 참조 버전
    bool FindTrianglePath(int startTriangle, int endTriangle, vector<int>& outPath); // 참조로 변경
    void ReconstructPath(const vector<PathNode>& allNodes, int endTriangle, vector<int>& outPath); // 참조로 변경

    // 경로 변환 및 최적화 - 성능 최적화된 참조 버전
    void ConvertTrianglePathToWorldPath(const vector<int>& trianglePath, const Vec3& start, const Vec3& end, vector<Vec3>& outPath); // 참조로 변경
    void SmoothPath(const vector<Vec3>& originalPath, vector<Vec3>& outPath); // 참조로 변경
    void SmoothPathOri(const vector<Vec3>& originalPath, vector<Vec3>& outPath);
    // 유틸리티 함수
    int FindTriangleContaining(const Vec3& point);
    Vec3 ProjectPointOnTriangle(const Vec3& point, const NavMeshTriangle& triangle);
    bool IsPointInTriangle(const Vec3& point, const NavMeshTriangle& triangle);
    bool HasLineOfSight(const Vec3& start, const Vec3& end);
    bool IsLineOnNavMesh(const Vec3& start, const Vec3& end, float stepSize = 0.5f);
    float GetDistance(const Vec3& a, const Vec3& b);
    int GetTotalNeighbors() const;
};