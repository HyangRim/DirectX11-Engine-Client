#include "pch.h"
#include "NickyWState.h"

#include "ModelAnimator.h"

#include "PlayerStateMachine.h"
#include "AnimationStateMachine.h"

NickyWState::NickyWState(shared_ptr<ModelAnimator> modelAnimator, shared_ptr<GameObject> _player)
    :Super(PlayerStateType::Skill_2)
    , m_modelAnimator(modelAnimator)
    , m_player(_player)
{

}

NickyWState::~NickyWState()
{

}

void NickyWState::Enter()
{
    m_skillTime = 0.0f;
    m_isAnimationStarted = true;
    m_isSkillComplete = false;

    cout << "NickyWState진입\n";
}

void NickyWState::Update()
{
        // 대기 시간 업데이트
    m_skillTime += DT;

    if (m_isSkillComplete)
    {
        // 스킬이 완료되면 자동으로 Wait 상태로 전환 요청
        // 실제 전환은 AnimationStateMachine에서 처리
        return;
    }

    // 시퀀스 재생 상태 체크
    if (!m_isSkillComplete && !m_modelAnimator->IsSequencePlaying())
    {
        // 시퀀스가 끝났으면 완료 플래그 설정
        m_isSkillComplete = true;
        cout << "W 스킬 시퀀스 자동 완료 감지" << endl;
    }
}

void NickyWState::Exit()
{
    // 상태 종료 시 정리
    m_skillTime = 0.0f;
    m_isAnimationStarted = false;
    m_isSkillComplete = false;

    cout << "NickyWState종료\n";
}

bool NickyWState::CanTransitionTo(PlayerStateType newState)
{
    //// 스킬이 완료되었을 때만 Wait 상태로 전환 가능
    //if (m_isSkillComplete && newState == PlayerStateType::Wait)
    //{
    //    return true;
    //}
    //return false;

    //반격은 바로
    if (newState == PlayerStateType::Counter)
        return true;


    if (!m_isSkillComplete)
        return false;

    switch (newState)
    {
    case PlayerStateType::Counter:
    case PlayerStateType::Wait:
        return true;
    default:
        return false;
    }

}
