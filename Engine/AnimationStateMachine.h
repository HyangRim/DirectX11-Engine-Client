#pragma once
#include "Component.h"
#include "AnimationState.h"

// 애니메이션 상태 머신
class AnimationStateMachine : public Component
{
    using Super = Component;

public:
    AnimationStateMachine();
    ~AnimationStateMachine();

    virtual void Start() override;
    virtual void Update() override;

    // 상태 전환
    void ChangeState(AnimationStateType newState);
    bool CanChangeState(AnimationStateType newState);

    // 상태 조회
    AnimationStateType GetCurrentState() const;
    shared_ptr<AnimationState> GetCurrentState2(AnimationStateType type);
    bool IsInState(AnimationStateType state) const;

    // 입력 처리
    void ProcessInput();
    void RegisterState(AnimationStateType type, shared_ptr<AnimationState> state);

    Ray CreateRayFromMouse(POINT mousePos, shared_ptr<Camera> camera);

private:
    void InitializeStates();
    void HandleSpecialStateTransitions();  // 추가
   
private:
    unordered_map<AnimationStateType, shared_ptr<AnimationState>> m_states;
    shared_ptr<AnimationState> m_currentState;
    shared_ptr<ModelAnimator> m_animator;


    // 입력 상태
    bool m_wasMoving = false;
    bool m_isChargingQ = false;
};