// AnimationStateMachine.cpp
#include "pch.h"
#include "AnimationStateMachine.h"
#include "GameObject.h"
#include "ModelAnimator.h"

// ... 다른 상태들 include

AnimationStateMachine::AnimationStateMachine()
    : Super(ComponentType::AnimationStateMachine)
{
}

AnimationStateMachine::~AnimationStateMachine()
{
}

void AnimationStateMachine::Start()
{
    Super::Start();

    auto gameObject = GetGameObject();
    if (gameObject)
    {
        m_animator = gameObject->GetModelAnimator();
    }

    InitializeStates();

    // 초기 상태 설정
    m_currentState = m_states[AnimationStateType::Wait];
    if (m_currentState)
    {
        m_currentState->Enter(m_animator);
    }
}

void AnimationStateMachine::Update()
{
    Super::Update();

    ProcessInput();

    if (m_currentState)
    {
        m_currentState->Update(m_animator);     

        // 특수 상태 처리 추가
        HandleSpecialStateTransitions();
    }
}

void AnimationStateMachine::ProcessInput()
{
    // 이동 입력 처리
    bool isMoving = INPUT->GetButton(KEY_TYPE::UP) ||
        INPUT->GetButton(KEY_TYPE::DOWN) ||
        INPUT->GetButton(KEY_TYPE::LEFT) ||
        INPUT->GetButton(KEY_TYPE::RIGHT);

    // 이동 상태 변화 처리
    if (isMoving && !m_wasMoving && CanChangeState(AnimationStateType::Run))
    {
        ChangeState(AnimationStateType::Run);
    }
    else if (!isMoving && m_wasMoving && CanChangeState(AnimationStateType::Wait))
    {
        ChangeState(AnimationStateType::Wait);
    }

    // 스킬 입력 처리
    if (INPUT->GetButtonDown(KEY_TYPE::B))
    {
        ChangeState(AnimationStateType::Skill_1);
        //m_isChargingQ = true;
    }
    // 스킬 입력 처리
    if (INPUT->GetButtonDown(KEY_TYPE::W))
    {
        ChangeState(AnimationStateType::Skill_2);
        //m_isChargingQ = true;
    }
    // 스킬 입력 처리
    if (INPUT->GetButtonDown(KEY_TYPE::E))
    {
        ChangeState(AnimationStateType::Skill_3);
        //m_isChargingQ = true;
    }
    // 스킬 입력 처리
    if (INPUT->GetButtonDown(KEY_TYPE::R))
    {
        ChangeState(AnimationStateType::Skill_4);
        //m_isChargingQ = true;
    }

    m_wasMoving = isMoving; 
}

void AnimationStateMachine::ChangeState(AnimationStateType newState)
{
    if (!CanChangeState(newState))
        return;

    // 현재 상태 종료
    if (m_currentState)
    {
        m_currentState->Exit(m_animator);
    }

    // 새 상태 시작
    m_currentState = m_states[newState];
    if (m_currentState)
    {
        m_currentState->Enter(m_animator);
    }
}

bool AnimationStateMachine::CanChangeState(AnimationStateType newState)
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}

AnimationStateType AnimationStateMachine::GetCurrentState() const
{
    return m_currentState ? m_currentState->GetType() : AnimationStateType::Wait;
}

bool AnimationStateMachine::IsInState(AnimationStateType state) const
{
    return GetCurrentState() == state;
}

void AnimationStateMachine::InitializeStates()
{
    // ... 다른 상태들 등록
}

// 새로운 메서드 추가
void AnimationStateMachine::HandleSpecialStateTransitions()
{
    // Q 스킬 완료 후 Wait 상태로 전환
    if (GetCurrentState() == AnimationStateType::Skill_1)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }
    // W 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == AnimationStateType::Skill_2)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }
    // E 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == AnimationStateType::Skill_3)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }
    // W 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == AnimationStateType::Skill_4)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }

    // 다른 스킬들도 동일하게 처리 가능
    // if (GetCurrentState() == AnimationStateType::Skill_1) { ... }
}

void AnimationStateMachine::RegisterState(AnimationStateType type, shared_ptr<AnimationState> state)
{
    m_states[type] = state;
}
