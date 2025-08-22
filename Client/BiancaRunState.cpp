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
    m_stepSoundTime += DT;

    if (m_stepSoundTime > 0.32f) {
        if (leftStep) {
            SOUND->PlaySound(L"SFX/footstepAsphalt_s1.wav", 14, 0.5f);
            leftStep = false;
        }
        else {
            SOUND->PlaySound(L"SFX/footstepAsphalt_s2.wav", 14, 0.5f);
            leftStep = true;
        }
        m_stepSoundTime = 0.f;
    }
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
    case PlayerStateType::Craft:
    case PlayerStateType::BaseAttack:
        return true;
    case PlayerStateType::Run:
        return false;  // 자기 자신으로는 전환 불가
    default:
        return false;
    }
}