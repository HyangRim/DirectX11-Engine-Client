#include "pch.h"
#include "WolfTraceState.h"

WolfTraceState::WolfTraceState()
    :Super(MonsterStateType::Wait)
{

}

void WolfTraceState::Enter()
{
    m_animTime = 0.f;
    m_isAnimationStarted = true;

    cout << "´Á´ë Trace State ÁøÀÔ\n";
}

void WolfTraceState::Update()
{

}

void WolfTraceState::Exit()
{
    m_animTime = 0.f;
    m_isAnimationStarted = false;

    cout << "´Á´ë Trace State Á¾·á\n";
}

bool WolfTraceState::CanTransitionTo(MonsterStateType newState)
{
    switch (newState)
    {
    case MonsterStateType::Attack:
    case MonsterStateType::Death:
    case MonsterStateType::Wait:
        return true;
    default:
        return false;
    }
}