#include "pch.h"
#include "NickyEState.h"
#include "ModelAnimator.h"

NickyEState::NickyEState(shared_ptr<ModelAnimator> modelAnimator)
    :Super(PlayerStateType::Skill_3)
    , m_modelAnimator(modelAnimator)
{

}

NickyEState::~NickyEState()
{

}

void NickyEState::Enter()
{
    m_skillTime = 0.0f;
    m_isAnimationStarted = true;
    m_isSkillComplete = false;

    m_durationTime = 0.f;

    cout << "NickyEState진입\n";
}

void NickyEState::Update()
{
    UpdateNormalSkill();
}

void NickyEState::Exit()
{
    // 상태 종료 시 정리
    m_skillTime = 0.0f;
    m_isAnimationStarted = false;
    m_isSkillComplete = false;

    cout << "NickyEState종료\n";
}

bool NickyEState::CanTransitionTo(PlayerStateType newState)
{
    // 스킬이 완료되었을 때만 Wait 상태로 전환 가능
    if (m_isSkillComplete && newState == PlayerStateType::Wait)
    {
        return true;
    }
    return false;
}

void NickyEState::UpdateNormalSkill()
{
    // 대기 시간 업데이트
    m_skillTime += DT;

    //if (m_isSkillComplete)
    //{
    //    // 스킬이 완료되면 자동으로 Wait 상태로 전환 요청
    //    // 실제 전환은 AnimationStateMachine에서 처리
    //    return;
    //}

    //// 시퀀스 재생 상태 체크
    //if (m_isAnimationStarted && !m_modelAnimator->IsSequencePlaying())
    //{
    //    // 시퀀스가 끝났으면 완료 플래그 설정
    //    m_isSkillComplete = true;
    //    cout << "E 스킬 시퀀스 자동 완료 감지" << endl;
    //}

     // 현재 애니메이션 확인
    //wstring currentAnim = m_modelAnimator->GetCurrentAnimationTag();

    //if (currentAnim == L"Skill_03")
    //{
    //    cout << "E 스킬 여긴데 \n";
    //    // End 애니메이션이 완료되면 스킬 완료
    //    bool isFinished = m_modelAnimator->IsAnimationFinished();
    //    if (isFinished)
    //    {
    //        cout << "State Nicky E 스킬 애니메이션 완료!" << endl;
    //        m_isSkillComplete = true;
    //    }
    //}
    if (m_isSkillComplete)
    {
        // 스킬이 완료되면 자동으로 Wait 상태로 전환 요청
        // 실제 전환은 AnimationStateMachine에서 처리
        return;
    }

    // 시퀀스 재생 상태 체크
    if (m_isAnimationStarted && !m_modelAnimator->IsSequencePlaying())
    {
        // 시퀀스가 끝났으면 완료 플래그 설정
        m_isSkillComplete = true;
        cout << "E 스킬 시퀀스 자동 완료 감지" << endl;
    }

}