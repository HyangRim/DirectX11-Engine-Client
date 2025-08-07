#include "pch.h"
#include "AlphaWaitState.h"

AlphaWaitState::AlphaWaitState()
    : AnimationState(AnimationStateType::Wait)
{
}

AlphaWaitState::~AlphaWaitState()
{
}

void AlphaWaitState::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // Wait 애니메이션 재생
    _animator->SetAnimationByTag(L"Wait", false);

    m_animTime = 0.0f;
    m_isAnimationStarted = true;

    cout << "Alpha Wait 상태 진입 - Wait 애니메이션 재생 시작" << endl;
}

void AlphaWaitState::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 대기 시간 업데이트
    m_animTime += DT;

    // 애니메이션이 정상적으로 재생되고 있는지 확인
    if (m_isAnimationStarted)
    {
        wstring currentAnimTag = _animator->GetCurrentAnimationTag();
        if (currentAnimTag == L"Wait")
        {
            // Wait 애니메이션이 정상적으로 재생 중
            // 필요시 추가 로직 구현
        }
    }
}

void AlphaWaitState::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "Wait 상태 종료 - 대기 시간: " << m_animTime << "초" << endl;

    // 상태 종료 시 정리
    m_animTime = 0.0f;
    m_isAnimationStarted = false;
}

bool AlphaWaitState::CanTransitionTo(AnimationStateType _nextState)
{
    // Wait 상태에서는 대부분의 상태로 전환 가능
    switch (_nextState)
    {
    case AnimationStateType::Move:
    case AnimationStateType::BaseAttack:
    case AnimationStateType::Skill_1:
    case AnimationStateType::Run:
    case AnimationStateType::Dying:
    case AnimationStateType::Dead:
        return true;
    case AnimationStateType::Wait:
        return false;  // 자기 자신으로는 전환 불가
    default:
        return false;
    }
}
