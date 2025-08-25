#pragma once
#include "Component.h"
#include "MonsterState.h"
#include "EventClass.h"

class MonsterStateMachine : public Component
{
    using Super = Component;

public:
    MonsterStateMachine();
    virtual ~MonsterStateMachine();

    // Component 메서드 오버라이드
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnDestroy() override;

    // 상태 관리
    void RequestStateChange(MonsterStateType newState);
    bool CanChangeState(MonsterStateType newState) const;

    // 상태 조회
    MonsterStateType GetCurrentState() const;
    shared_ptr<MonsterState> GetCurrentStatePtr() const;
    bool IsInState(MonsterStateType state) const;

    // 상태 등록
    void RegisterState(MonsterStateType type, shared_ptr<MonsterState> state);

    // AI 관련
    void SetTarget(shared_ptr<GameObject> target);
    shared_ptr<GameObject> GetTarget() const;

private:
    // 상태 전환 실제 실행
    void ExecuteStateChange(MonsterStateType newState);

    // 이벤트 핸들러
    void HandleStateChangeRequest(shared_ptr<EventData> eventData);
    void HandleAnimationStateChanged(shared_ptr<EventData> eventData);

    // AI 로직
    void ProcessAI();
    void UpdateTargetDetection();

private:
    // 상태 관리
    unordered_map<MonsterStateType, shared_ptr<MonsterState>> m_states;
    shared_ptr<MonsterState> m_currentState;

    // 컴포넌트 참조
    shared_ptr<class AnimationStateMachine> m_animationStateMachine;
    shared_ptr<class NavMeshAgent> m_navMeshAgent;

    // AI 관련
    shared_ptr<GameObject> m_target;
    float m_detectionRange = 10.0f;
    float m_attackRange = 2.0f;

    // 상태 전환 대기열
    queue<MonsterStateType> m_stateChangeQueue;

    // 디버그
    bool m_enableDebugLog = false;
};
