#include "pch.h"
#include "AlphaDeathState.h"

AlphaDeathState::AlphaDeathState()
    : AnimationState(AnimationStateType::Dead)
{
}

AlphaDeathState::~AlphaDeathState()
{
}

void AlphaDeathState::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // Wait 애니메이션 재생
    _animator->SetAnimationByTag(L"Death", false);

    m_animTime = 0.0f;
    m_isAnimationStarted = true;

    cout << "Alpha Death 상태 진입 - Death 애니메이션 재생 시작" << endl;
}

void AlphaDeathState::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 대기 시간 업데이트
    m_animTime += DT;

    // 애니메이션이 정상적으로 재생되고 있는지 확인
    if (m_isAnimationStarted)
    {
        wstring currentAnimTag = _animator->GetCurrentAnimationTag();
        if (currentAnimTag == L"Death")
        {
            // Wait 애니메이션이 정상적으로 재생 중
            // 필요시 추가 로직 구현
        }
    }
}

void AlphaDeathState::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "Death 상태 종료 - 대기 시간: " << m_animTime << "초" << endl;

    // 상태 종료 시 정리
    m_animTime = 0.0f;
    m_isAnimationStarted = false;
}

bool AlphaDeathState::CanTransitionTo(AnimationStateType _nextState)
{
    switch (_nextState)
    {
    case AnimationStateType::Dying:
        return true;
    default:
        return false;
    }
}
