#include "pch.h"
#include "WolfDyingState.h"

WolfDyingState::WolfDyingState()
	: AnimationState(AnimationStateType::Dead)
{
}

void WolfDyingState::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // Wait 애니메이션 재생
    _animator->SetAnimationByTag(L"Dying", true);

    m_dyingTime = 0.0f;
    m_isAnimationStarted = true;

    cout << "Wait 상태 진입 - Wait 애니메이션 재생 시작" << endl;
}

void WolfDyingState::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 대기 시간 업데이트
    m_dyingTime += DT;

    // 애니메이션이 정상적으로 재생되고 있는지 확인
    if (m_isAnimationStarted)
    {
        wstring currentAnimTag = _animator->GetCurrentAnimationTag();
        if (currentAnimTag == L"Dying")
        {
            // Wait 애니메이션이 정상적으로 재생 중
            // 필요시 추가 로직 구현
        }
    }
}

void WolfDyingState::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "Dying 상태 종료 - 대기 시간: " << m_dyingTime << "초" << endl;

    // 상태 종료 시 정리
    m_dyingTime = 0.0f;
    m_isAnimationStarted = false;
}

bool WolfDyingState::CanTransitionTo(AnimationStateType _nextState)
{
    return false;
}
