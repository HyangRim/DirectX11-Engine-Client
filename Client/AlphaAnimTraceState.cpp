#include "pch.h"
#include "AlphaAnimTraceState.h"

AlphaAnimTraceState::AlphaAnimTraceState()
	: AnimationState(AnimationStateType::Trace)
{
}

void AlphaAnimTraceState::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;
    _animator->SetAnimationSpeed(m_playSpeed);
    // Wait 애니메이션 재생
    _animator->SetAnimationByTag(L"Run", false);

    m_idleTime = 0.0f;
    m_isAnimationStarted = true;

    cout << "알파 Run 애니메이션 재생 시작" << endl;
}

void AlphaAnimTraceState::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 대기 시간 업데이트
    m_idleTime += DT;

    // 애니메이션이 정상적으로 재생되고 있는지 확인
    if (m_isAnimationStarted)
    {
        wstring currentAnimTag = _animator->GetCurrentAnimationTag();
        if (currentAnimTag == L"Run")
        {
            //cout << "Run애니메이션 재생중...\n";
            // Wait 애니메이션이 정상적으로 재생 중
            // 필요시 추가 로직 구현
        }
    }
}

void AlphaAnimTraceState::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "알파 Run 상태 종료 " << endl;

    // 상태 종료 시 정리
    m_idleTime = 0.0f;
    m_isAnimationStarted = false;
    _animator->SetAnimationSpeed(1.f);
}

bool AlphaAnimTraceState::CanTransitionTo(AnimationStateType _nextState)
{
    // Wait 상태에서는 대부분의 상태로 전환 가능
    switch (_nextState)
    {
    case AnimationStateType::Move:
    case AnimationStateType::BaseAttack:
    case AnimationStateType::Run:
    case AnimationStateType::Dying:
    case AnimationStateType::Death:
        return true;
    case AnimationStateType::Wait:
        return false;  // 자기 자신으로는 전환 불가
    default:
        return false;
    }
}
