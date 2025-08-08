#include "pch.h"
#include "WolfAnimDyingState.h"

WolfAnimDyingState::WolfAnimDyingState()
	: AnimationState(AnimationStateType::Death)
{
}

void WolfAnimDyingState::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    _animator->SetAnimationSpeed(m_playSpeed);
    m_expectedDuration = _animator->GetAnimationDuration(L"Dying") / m_playSpeed;
    _animator->SetAnimationSpeed(m_playSpeed);

    // Wait 애니메이션 재생
    //_animator->SetAnimationByTag(L"Dying", true);
    _animator->PlaySequence(L"Wolf_dying_Sequence");

    m_dyingTime = 0.0f;
    m_isAnimationStarted = true;
    m_isDyingComplete = false;
    cout << "늑대 Dying 애니메이션 재생 시작" << endl;
}

void WolfAnimDyingState::Update(shared_ptr<ModelAnimator> _animator)
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

    // 시간 기반으로 완료 체크
    if (!m_isDyingComplete && m_dyingTime >= m_expectedDuration)
    {
        m_isDyingComplete = true;
        // 안전하게 시퀀스 정지
        //_animator->StopSequence();
        cout << "늑대 시체됨 !" << endl;
    }
}

void WolfAnimDyingState::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "늑대 Dying 상태 종료" << endl;

    // 상태 종료 시 정리
    m_dyingTime = 0.0f;
    m_isAnimationStarted = false;
    _animator->SetAnimationSpeed(1.f);
}

bool WolfAnimDyingState::CanTransitionTo(AnimationStateType _nextState)
{
    return false;
}
