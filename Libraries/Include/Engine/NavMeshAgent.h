#pragma once
#include "Component.h"
#include "NavMesh.h"

enum class NavMeshAgentState
{
    Idle,
    Moving,
    Arrived
};

class NavMeshAgent : public Component
{
    using Super = Component;

public:
    NavMeshAgent();
    virtual ~NavMeshAgent();

    virtual void Start() override;
    virtual void Update() override;

    // 핵심 기능
    void SetDestination(const Vec3& destination);
    void Stop();

    // 상태 확인
    NavMeshAgentState GetState() const { return m_state; }
    bool IsMoving() const { return m_state == NavMeshAgentState::Moving; }
    bool HasReachedDestination() const { return m_state == NavMeshAgentState::Arrived; }

    // 설정
    void SetSpeed(float speed) { m_speed = speed; }
    float GetSpeed() const { return m_speed; }

    // 벤치마크용: 씬 검색 없이 NavMesh 지정 / 고정 dt 로 이동만 수행 / 마지막 이동 요청
    void SetNavMesh(shared_ptr<NavMesh> navMesh) { m_navMesh = navMesh; }
    void Simulate(float dt);
    bool GetLastRequest(Vec3& outStart, Vec3& outDestination) const;
    void SetStoppingDistance(float distance) { m_stoppingDistance = distance; }

    // Look-ahead 추종: 웨이포인트에 닿을 때까지 직진하지 않고, 경로 위에서 L 만큼 앞의 점(목표점)을 향해 이동한다.
    //  → 코너 앞에서 미리, 둥글게 꺾는다. 경로(중점 기반 Corridor)는 그대로. 0 = 끔 (기존 동작, 몬스터 기본값)
    void SetLookAhead(float distance) { m_lookAhead = max(0.f, distance); }
    float GetLookAhead() const { return m_lookAhead; }

    // ---------------------------------------------------------------------
    // 경로 진단 (플레이어만 켜서 사용)
    //  - 이동 요청마다 FindPath 분기 · 경로가 NavMesh 밖으로 나가는 지점을 로그로 출력
    //  - 매 프레임 위치 보정(스냅)이 크게 일어난 지점 기록
    //  - DrawPathDebug: 노란색 = A* 중심점 경로, 하늘색 = 최종 경로,
    //                   자홍색 = 경로가 NavMesh 밖인 지점, 주황색 = 이동 중 보정 지점
    // ---------------------------------------------------------------------
    void SetPathDebugEnabled(bool enabled) { m_pathDebugEnabled = enabled; }
    bool IsPathDebugEnabled() const { return m_pathDebugEnabled; }
    void DrawPathDebug(const Matrix& view, const Matrix& projection);

private:
    void UpdateMovement(float dt);
    void UpdateMovementLookAhead(float dt);
    Vec3 ComputeLookAheadTarget(const Vec3& pos, float lookAhead) const;
    void MoveToward(const Vec3& currentPos, const Vec3& targetPos, float dt);
    void UpdateAnimation();

    void CapturePathDebug();
    void RecordSnap(const Vec3& before, const Vec3& after);

private:
    // 경로찾기 관련
    shared_ptr<NavMesh> m_navMesh;
    vector<Vec3> m_path;
    uint32 m_currentPathIndex = 0;
    Vec3 m_destination;
    NavMeshAgentState m_state = NavMeshAgentState::Idle;

    // 이동 설정
    float m_speed = 5.0f;
    float m_stoppingDistance = 0.1f;
    float m_lookAhead = 0.f;          // Look-ahead 거리 (0 = 끔)
    Vec3 m_lookAheadTarget;           // 마지막 목표점 (진단 표시용)
    bool m_hasLookAheadTarget = false;

    // 애니메이션
    shared_ptr<ModelAnimator> m_animator;

    // 마지막 이동 요청 (F10 코너 케이스 저장용)
    bool m_hasLastRequest = false;
    Vec3 m_lastRequestStart;
    Vec3 m_lastRequestDestination;

    // 경로 진단
    bool m_pathDebugEnabled = false;
    NavPathDebugInfo m_lastPathDebug;
    vector<Vec3> m_offMeshSamples;          // 최종 경로 위에서 NavMesh 밖으로 판정된 샘플
    vector<Vec3> m_snapPoints;              // 이동 중 보정이 크게 일어난 위치 (최근 N개)
    vector<Vec3> m_trail;                   // 실제 이동 궤적 (진단 표시용)
    size_t m_snapWriteIndex = 0;
    int m_snapCount = 0;
};