#include "pch.h"
#include "BiancaAnimBaseAttackState.h"
#include "ModelAnimator.h"

#include "BiancaBaseAttack.h"

BiancaAnimBaseAttackState::BiancaAnimBaseAttackState()
    : AnimationState(AnimationStateType::BaseAttack)

{

}

void BiancaAnimBaseAttackState::Enter(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;

    animator->SetAnimationSpeed(m_playSpeed);

    if (m_motionChange)
    {
        /*m_sequenceDurations = animator->GetSequenceAnimationDurations(L"Wolf_Atk1_Sequence");
        for (size_t i = 0; i < m_sequenceDurations.size(); i++)
        {
            m_sequenceDurations[i] /= m_playSpeed;
        }
        animator->SetSequenceAnimationDurations(L"Wolf_Atk1_Sequence", m_sequenceDurations);
        animator->PlaySequence(L"Wolf_Atk1_Sequence");*/

        animator->SetAnimationByTag(L"BaseAttack_01", true);

        m_motionChange = !m_motionChange;
    }
    else
    {
        /*m_sequenceDurations = animator->GetSequenceAnimationDurations(L"Wolf_Atk2_Sequence");
        for (size_t i = 0; i < m_sequenceDurations.size(); i++)
        {
            m_sequenceDurations[i] /= m_playSpeed;
        }
        animator->SetSequenceAnimationDurations(L"Wolf_Atk2_Sequence", m_sequenceDurations);
        animator->PlaySequence(L"Wolf_Atk2_Sequence");*/

        animator->SetAnimationByTag(L"BaseAttack_02", true);
        m_motionChange = !m_motionChange;
    }

    m_skillTime = 0.0f;
    m_isAnimationStarted = true;
    m_isSkillComplete = false;

    cout << "Nicky BaseAttack 애니메이션 재생 시작" << endl;
}

void BiancaAnimBaseAttackState::Update(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;

    if (animator->GetGameObject()->GetComponent<BiancaBaseAttack>()->GetTarget() == nullptr)
    {
        m_isSkillComplete = true;
        return;
    }

    m_skillTime += DT;
    // 시간 기반으로 완료 체크
    if (!m_isSkillComplete && m_skillTime >= (38.f / 25.f) / 2.f)
    {
        m_isSkillComplete = true;
        // 시퀀스 정지
        //animator->StopSequence();
        wcout << L"BaseAttack 시간 기반 완료!" << endl;
    }
}

void BiancaAnimBaseAttackState::Exit(shared_ptr<ModelAnimator> animator)
{
    if (!animator)
        return;
    animator->SetAnimationSpeed(1.f);

    cout << "Nicky BaseAttack 애니메이션 종료" << endl;

    // 상태 종료 시 정리
    m_skillTime = 0.0f;
    m_isAnimationStarted = false;
    m_isSkillComplete = false;
    m_cachedAnimator.reset();
}

bool BiancaAnimBaseAttackState::CanTransitionTo(AnimationStateType nextState)
{
    //// 스킬이 완료되었을 때만 Wait 상태로 전환 가능
    //if (m_isSkillComplete && nextState == AnimationStateType::Wait)
    //{
    //    return true;
    //}
    //return false;

    switch (nextState)
    {
    case AnimationStateType::Wait:
    case AnimationStateType::BaseAttack:
    case AnimationStateType::Skill_1:
    case AnimationStateType::Skill_2:
    case AnimationStateType::Skill_3:
    case AnimationStateType::Skill_4:
        return true;
    case AnimationStateType::Run:

        return true;
    default:
        return false;
    }
}
