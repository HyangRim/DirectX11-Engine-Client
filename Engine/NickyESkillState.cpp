#include "pch.h"
#include "NickyESkillState.h"
#include "ModelAnimator.h"

NickyESkillState::NickyESkillState()
	: AnimationState(AnimationStateType::Skill_3)
{
}

void NickyESkillState::Enter(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;

    animator->SetAnimationSpeed(m_playSpeed);
    m_expectedDuration = animator->GetAnimationDuration(L"Skill_03") / m_playSpeed;
    // 스킬 시퀀스 재생
    animator->PlaySequence(L"Skill_3_Sequence");

    // 시퀀스 완료 콜백 설정
    animator->SetSequenceCompleteCallback(L"Skill_3_Sequence", [this]() {
        m_isSkillComplete = true;  // 스킬 완료 플래그 설정
        wcout << L"E 스킬 시퀀스 완료!" << endl;
    });

    m_skillTime = 0.0f;
    m_isAnimationStarted = true;
    m_isSkillComplete = false;

    //cout << "Skill3 상태 진입 - Skill3 애니메이션 재생 시작" << endl;
}

void NickyESkillState::Update(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;

    //// 대기 시간 업데이트
    //m_skillTime += DT;

    //if (m_isSkillComplete)
    //{
    //    // 스킬이 완료되면 자동으로 Wait 상태로 전환 요청
    //    // 실제 전환은 AnimationStateMachine에서 처리
    //    return;
    //}

    //// 시퀀스 재생 상태 체크
    //if (m_isAnimationStarted && !animator->IsSequencePlaying())
    //{
    //    // 시퀀스가 끝났으면 완료 플래그 설정
    //    m_isSkillComplete = true;
    //    wcout << L"E 스킬 시퀀스 자동 완료 감지" << endl;
    //}

    m_skillTime += DT;

    // 시간 기반으로 완료 체크
    if (!m_isSkillComplete && m_skillTime >= m_expectedDuration)
    {
        m_isSkillComplete = true;
        // 안전하게 시퀀스 정지
        animator->StopSequence();
        wcout << L"E 스킬 시간 기반 완료!" << endl;
    }
}

void NickyESkillState::Exit(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;
    animator->SetAnimationSpeed(1.f);

    //cout << "Skill3 상태 종료 - 대기 시간: " << m_skillTime << "초" << endl;

    // 상태 종료 시 정리
    m_skillTime = 0.0f;
    m_isAnimationStarted = false;
    m_isSkillComplete = false;
    m_cachedAnimator.reset();
}

bool NickyESkillState::CanTransitionTo(AnimationStateType nextState)
{
    // 스킬이 완료되었을 때만 Wait 상태로 전환 가능
    if (m_isSkillComplete && nextState == AnimationStateType::Wait)
    {
        return true;
    }
    return false;
}
