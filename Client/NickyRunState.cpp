#include "pch.h"
#include "NickyRunState.h"

NickyRunState::NickyRunState()
    :Super(PlayerStateType::Run)
{

}

NickyRunState::~NickyRunState()
{
}


void NickyRunState::Enter()
{
    cout << "NickyRunState진입\n";
}

void NickyRunState::Update()
{

}

void NickyRunState::Exit()
{
    cout << "NickyRunState종료\n";
}

bool NickyRunState::CanTransitionTo(PlayerStateType newState)
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