#include "pch.h"
#include "AlphaAppearState.h"

AlphaAppearState::AlphaAppearState()
    : AnimationState(AnimationStateType::Appear)
{
}

AlphaAppearState::~AlphaAppearState()
{
}

void AlphaAppearState::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // Wait 애니메이션 재생
    _animator->SetAnimationByTag(L"Appear", false);

    m_animTime = 0.0f;
    m_isAnimationStarted = true;

    cout << "Alpha Appear 상태 진입 - Appear 애니메이션 재생 시작" << endl;
}

void AlphaAppearState::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 대기 시간 업데이트
    m_animTime += DT;

    // 애니메이션이 정상적으로 재생되고 있는지 확인
    if (m_isAnimationStarted)
    {
        wstring currentAnimTag = _animator->GetCurrentAnimationTag();
        if (currentAnimTag == L"Appear")
        {
            // Wait 애니메이션이 정상적으로 재생 중
            // 필요시 추가 로직 구현
        }
    }
}

void AlphaAppearState::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "Alpha Appear 상태 종료 - 대기 시간: " << m_animTime << "초" << endl;

    // 상태 종료 시 정리
    m_animTime = 0.0f;
    m_isAnimationStarted = false;
}

bool AlphaAppearState::CanTransitionTo(AnimationStateType _nextState)
{
    // 스킬이 완료되었을 때만 Wait 상태로 전환 가능
    if (m_isAppearComplete && _nextState == AnimationStateType::Wait)
    {
        return true;
    }
    return false;
}
