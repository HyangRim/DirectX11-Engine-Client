#pragma once
#include "Component.h"

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

    // 목표 지점으로 이동 시작
    void SetDestination(const Vec3& destination);

    // 이동 중지
    void Stop();

    // 현재 상태 확인
    NavMeshAgentState GetState() const { return m_state; }
    bool IsMoving() const { return m_state == NavMeshAgentState::Moving; }
    bool HasReachedDestination() const { return m_state == NavMeshAgentState::Arrived; }

    // 설정
    void SetSpeed(float speed) { m_speed = speed; }
    void SetStoppingDistance(float distance) { m_stoppingDistance = distance; }

private:
    void UpdateMovement();
    void UpdateAnimation();

private:
    shared_ptr<NavMesh> m_navMesh;
    vector<Vec3> m_path;
    uint32 m_currentPathIndex = 0;

    Vec3 m_destination;
    NavMeshAgentState m_state = NavMeshAgentState::Idle;

    float m_speed = 5.0f;
    float m_stoppingDistance = 0.1f;

    // 애니메이션 관련
    shared_ptr<ModelAnimator> m_animator;
};
