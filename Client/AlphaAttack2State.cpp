#include "pch.h"
#include "AlphaAttack2State.h"

AlphaAttack2State::AlphaAttack2State()
    : AnimationState(AnimationStateType::BaseAttack)
{
}

AlphaAttack2State::~AlphaAttack2State()
{
}

void AlphaAttack2State::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // Wait 애니메이션 재생
    _animator->SetAnimationByTag(L"Atk2", false);

    m_animTime = 0.0f;
    m_isAnimationStarted = true;

    cout << "Alpha Atk2 상태 진입 - Atk2 애니메이션 재생 시작" << endl;
}

void AlphaAttack2State::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 대기 시간 업데이트
    m_animTime += DT;

    // 애니메이션이 정상적으로 재생되고 있는지 확인
    if (m_isAnimationStarted)
    {
        wstring currentAnimTag = _animator->GetCurrentAnimationTag();
        if (currentAnimTag == L"Atk2")
        {
            // Wait 애니메이션이 정상적으로 재생 중
            // 필요시 추가 로직 구현
        }
    }
}

void AlphaAttack2State::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "Alpha Atk2 상태 종료 - 대기 시간: " << m_animTime << "초" << endl;

    // 상태 종료 시 정리
    m_animTime = 0.0f;
    m_isAnimationStarted = false;
}

bool AlphaAttack2State::CanTransitionTo(AnimationStateType _nextState)
{
    // Wait 상태에서는 대부분의 상태로 전환 가능
    switch (_nextState)
    {
    case AnimationStateType::Move:
    case AnimationStateType::Run:
    case AnimationStateType::Wait:
        return true;
    case AnimationStateType::BaseAttack:
        return true;  // 자기 자신으로도 이건 전환 가능. 
    default:
        return false;
    }
}
