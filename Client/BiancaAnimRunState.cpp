#include "pch.h"
#include "BiancaAnimRunState.h"
#include "ModelAnimator.h"

BiancaAnimRunState::BiancaAnimRunState()
    : AnimationState(AnimationStateType::Move)
{
}

void BiancaAnimRunState::Enter(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;

    // Run 애니메이션 재생
    animator->SetAnimationByTag(L"Run", false);  // 부드러운 전환
    animator->SetAnimationSpeed(2.f);
    m_moveTime = 0.0f;
    m_isAnimationStarted = true;

    cout << "Bianca Run 애니메이션 재생 시작" << endl;
}

void BiancaAnimRunState::Update(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;

    m_moveTime += DT;

    // 이동 애니메이션이 정상적으로 재생되고 있는지 확인
    if (m_isAnimationStarted)
    {
        wstring currentAnimTag = animator->GetCurrentAnimationTag();
        if (currentAnimTag == L"Run")
        {
            // Run 애니메이션이 정상적으로 재생 중
        }
    }
}

void BiancaAnimRunState::Exit(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;

    cout << "Biaca Run 애니메이션 종료 " << endl;
    animator->SetAnimationSpeed(1.f);
    m_moveTime = 0.0f;
    m_isAnimationStarted = false;
}

bool BiancaAnimRunState::CanTransitionTo(AnimationStateType nextState)
{
    switch (nextState)
    {
    case AnimationStateType::Wait:
    case AnimationStateType::BaseAttack:
    case AnimationStateType::Charging:
    case AnimationStateType::Skill_1:
    case AnimationStateType::Skill_2:
    case AnimationStateType::Skill_3:
    case AnimationStateType::Skill_4:
        return true;
    case AnimationStateType::Run:
        return false;  // 자기 자신으로는 전환 불가
    default:
        return false;
    }
}
