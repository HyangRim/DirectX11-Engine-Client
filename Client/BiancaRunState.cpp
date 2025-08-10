#include "pch.h"
#include "BiancaRunState.h"

BiancaRunState::BiancaRunState()
    :Super(PlayerStateType::Run)
{

}

BiancaRunState::~BiancaRunState()
{
}


void BiancaRunState::Enter()
{
    cout << "BiancaRunState진입\n";
}

void BiancaRunState::Update()
{

}

void BiancaRunState::Exit()
{
    cout << "BiancaRunState종료\n";
}

bool BiancaRunState::CanTransitionTo(PlayerStateType newState)
{
    switch (newState)
    {
    case PlayerStateType::Wait:
    case PlayerStateType::Skill_1:
    case PlayerStateType::Skill_2:
    case PlayerStateType::Skill_3:
    case PlayerStateType::Skill_4:
        return true;
    case PlayerStateType::Run:
        return false;  // 자기 자신으로는 전환 불가
    default:
        return false;
    }
}