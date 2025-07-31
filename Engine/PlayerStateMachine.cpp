#include "pch.h"
#include "PlayerStateMachine.h"

PlayerStateMachine::PlayerStateMachine()
    : Component(ComponentType::Script) // or 적절한 타입 지정
    , m_currentState(nullptr)
{
}

PlayerStateMachine::~PlayerStateMachine()
{
    
}

void PlayerStateMachine::Init()
{
    // 초기화 작업
}

void PlayerStateMachine::Start()
{
    //ChangeState("Wait");
}

void PlayerStateMachine::Update()
{
    if (m_currentState)
    {
        m_currentState->Update();
    }
}

void PlayerStateMachine::OnDestroy()
{
    if (m_currentState)
    {
        m_currentState->Exit();
        m_currentState = nullptr;
    }
    m_states.clear();
}

void PlayerStateMachine::ChangeState(PlayerStateType newState)
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

bool PlayerStateMachine::CanChangeState(PlayerStateType newState)
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}

void PlayerStateMachine::RegisterState(PlayerStateType type, shared_ptr<PlayerState> state)
{
    m_states[type] = state;
}
