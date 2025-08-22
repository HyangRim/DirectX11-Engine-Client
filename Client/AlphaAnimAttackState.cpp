#include "pch.h"
#include "AlphaAnimAttackState.h"

AlphaAnimAttackState::AlphaAnimAttackState()
    : AnimationState(AnimationStateType::BaseAttack)
{
}

void AlphaAnimAttackState::Enter(shared_ptr<ModelAnimator> _animator)
{
	if (!_animator)
		return;

	_animator->SetAnimationSpeed(m_playSpeed);


    if (m_motionChange)
    {
        m_sequenceDurations = _animator->GetSequenceAnimationDurations(L"Alpha_Atk1_Sequence");
        for (size_t i = 0; i < m_sequenceDurations.size(); i++)
        {
            m_sequenceDurations[i] /= m_playSpeed;
        }
        _animator->SetSequenceAnimationDurations(L"Alpha_Atk1_Sequence", m_sequenceDurations);
        _animator->PlaySequence(L"Alpha_Atk1_Sequence");
        m_motionChange = !m_motionChange;
    }
    else
    {
        m_sequenceDurations = _animator->GetSequenceAnimationDurations(L"Alpha_Atk2_Sequence");
        for (size_t i = 0; i < m_sequenceDurations.size(); i++)
        {
            m_sequenceDurations[i] /= m_playSpeed;
        }
        _animator->SetSequenceAnimationDurations(L"Alpha_Atk2_Sequence", m_sequenceDurations);
        _animator->PlaySequence(L"Alpha_Atk2_Sequence");

        m_motionChange = !m_motionChange;
    }

	SOUND->PlaySound(L"Alpha/AlphaOmega_atk01.wav", 2, 0.5f);
	cout << "알파 Attack 애니메이션 시작." << endl;
}

void AlphaAnimAttackState::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 대기 시간 업데이트
    //m_animTime += DT;

    if (m_isAnimationStarted)
    {
        // 스킬이 완료되면 자동으로 Wait 상태로 전환 요청
        // 실제 전환은 AnimationStateMachine에서 처리
        return;
    }

    // 시퀀스 재생 상태 체크
    if (m_isAnimationStarted && !_animator->IsSequencePlaying())
    {
        // 시퀀스가 끝났으면 완료 플래그 설정
       // m_isAppearComplete = true;
        cout << "알파 Attack 애니메이션 완료." << endl;
    }
}

void AlphaAnimAttackState::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "알파 Attack 애니메이션 종료 " << endl;

    // 상태 종료 시 정리
    //m_animTime = 0.0f;
    m_isAnimationStarted = false;
    // m_isAppearComplete = false;

     //재생속도에 따라 애니메이션 속도들 원상복구  
    _animator->SetAnimationSpeed(1.f);
    for (size_t i = 0; i < m_sequenceDurations.size(); i++)
    {
        m_sequenceDurations[i] *= m_playSpeed;
    }
    if (m_motionChange)
    {
        _animator->SetSequenceAnimationDurations(L"Alpha_Atk1_Sequence", m_sequenceDurations);
    }
    else
    {
        _animator->SetSequenceAnimationDurations(L"Alpha_Atk2_Sequence", m_sequenceDurations);
    }
}

bool AlphaAnimAttackState::CanTransitionTo(AnimationStateType _nextState)
{
    switch (_nextState)
    {
    case AnimationStateType::Wait:
    case AnimationStateType::Trace:
    case AnimationStateType::BaseAttack:
    case AnimationStateType::Death:
        return true;
    default:
        return false;
    }
}
