#include "pch.h"
#include "AlphaWalkState.h"

AlphaWalkState::AlphaWalkState()
    : AnimationState(AnimationStateType::Move)
{
}

AlphaWalkState::~AlphaWalkState()
{
}

void AlphaWalkState::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // Run 애니메이션 재생
    _animator->SetAnimationByTag(L"Run", false);  // 부드러운 전환

    m_animTime = 0.0f;
    m_isAnimationStarted = true;

    cout << "Alpha Run 상태 진입 - Run 애니메이션 재생 시작" << endl;
}

void AlphaWalkState::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    m_animTime += DT;

    // 이동 애니메이션이 정상적으로 재생되고 있는지 확인
    if (m_isAnimationStarted)
    {
        wstring currentAnimTag = _animator->GetCurrentAnimationTag();
        if (currentAnimTag == L"Run")
        {
            // Run 애니메이션이 정상적으로 재생 중
        }
    }
}

void AlphaWalkState::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "Run 상태 종료 - 이동 시간: " << m_animTime << "초" << endl;

    m_animTime = 0.0f;
    m_isAnimationStarted = false;
}

bool AlphaWalkState::CanTransitionTo(AnimationStateType _nextState)
{
    switch (_nextState)
    {
    case AnimationStateType::Move:
    case AnimationStateType::Wait:
    case AnimationStateType::BaseAttack:
    case AnimationStateType::Dead:
    case AnimationStateType::Dying:
        return true;
    case AnimationStateType::Run:
        return false;  // 자기 자신으로는 전환 불가
    default:
        return false;
    }
}
