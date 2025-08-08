#include "pch.h"
#include "MonsterStateMachine.h"
#include "AnimationStateMachine.h"

MonsterStateMachine::MonsterStateMachine(shared_ptr<AnimationStateMachine> animationStateMachine)
	: Component(ComponentType::MonsterStateMachine)
    , m_animationStateMachine(animationStateMachine)
{
}

MonsterStateMachine::~MonsterStateMachine()
{
}

void MonsterStateMachine::Start()
{
    m_currentState = m_states[MonsterStateType::Appear];
    if (m_currentState)
    {
        m_currentState->Enter();
    }
}

void MonsterStateMachine::Update()
{
    ProcessInput();

    if (m_currentState)
    {
        m_currentState->Update();
        HandleSpecialStateTransitions();
    }
}

void MonsterStateMachine::ChangeState(MonsterStateType newState)
{
    if (!CanChangeState(newState))
        return;

    // 현재 상태 종료
    if (m_currentState)
    {
        m_currentState->Exit();
    }

    // 새 상태 시작
    m_currentState = m_states[newState];
    if (m_currentState)
    {
        m_currentState->Enter();
    }
}

bool MonsterStateMachine::CanChangeState(MonsterStateType newState)
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}

MonsterStateType MonsterStateMachine::GetCurrentState() const
{
    return m_currentState ? m_currentState->GetType() : MonsterStateType::Wait;
}

void MonsterStateMachine::RegisterState(MonsterStateType type, shared_ptr<MonsterState> state)
{
    m_states[type] = state;
}

void MonsterStateMachine::ProcessInput()
{
    if (INPUT->GetButtonDown(KEY_TYPE::KEY_5)) {
        ChangeState(MonsterStateType::Appear);
        m_animationStateMachine->ChangeState(AnimationStateType::Appear);
    }
    else if (INPUT->GetButtonDown(KEY_TYPE::KEY_6)) {
        ChangeState(MonsterStateType::Wait);
        m_animationStateMachine->ChangeState(AnimationStateType::Wait);
    }
    else if (INPUT->GetButtonDown(KEY_TYPE::KEY_7)) {
  
    }
    else if (INPUT->GetButtonDown(KEY_TYPE::KEY_8)) {
        ChangeState(MonsterStateType::Death);
        m_animationStateMachine->ChangeState(AnimationStateType::Death);
    }
}

// MonsterStateMachine.cpp의 HandleSpecialStateTransitions() 추가
void MonsterStateMachine::HandleSpecialStateTransitions()
{
    // Appear 상태 완료 후 Wait 상태로 전환
    if (GetCurrentState() == MonsterStateType::Appear)
    {
        // CanTransitionTo로 전환 가능 여부 체크
        if (m_currentState && m_currentState->CanTransitionTo(MonsterStateType::Wait))
        {
            cout << "Appear 상태 완료 조건 만족 - Wait로 전환" << endl;
            ChangeState(MonsterStateType::Wait);
            m_animationStateMachine->ChangeState(AnimationStateType::Wait);
        }
    }
    else if (GetCurrentState() == MonsterStateType::Death)
    {
        // CanTransitionTo로 전환 가능 여부 체크
        // 어차피 LogicState에서 완료를 판정했으므로 전환해도 무방
        if (m_currentState && m_currentState->CanTransitionTo(MonsterStateType::Dying))
        {
            cout << "Death 상태 완료 조건 만족 - Dying로 전환" << endl;
            ChangeState(MonsterStateType::Dying);
            m_animationStateMachine->ChangeState(AnimationStateType::Dying);
        }
    }
}
