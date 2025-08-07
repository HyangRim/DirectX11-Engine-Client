#include "pch.h"
#include "AlphaSkill1State.h"

AlphaSkill1State::AlphaSkill1State()
    : AnimationState(AnimationStateType::Skill_1)
{
}

AlphaSkill1State::~AlphaSkill1State()
{
}

void AlphaSkill1State::Enter(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 스킬 시퀀스 재생
    _animator->PlaySequence(L"Alpha_Skill1_Sequence");

    // 시퀀스 완료 콜백 설정
    _animator->SetSequenceCompleteCallback(L"Alpha_Skill1_Sequence", [this]() {
        m_isAppearComplete = true;  // 스킬 완료 플래그 설정
        wcout << L"Alpha Skill1 시퀸스 완료." << endl;
    });

    m_animTime = 0.0f;
    m_isAnimationStarted = true;
    m_isAppearComplete = false;

    cout << "Alpha Skill1 상태, 애니메이션 시작." << endl;
}

void AlphaSkill1State::Update(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    // 대기 시간 업데이트
    m_animTime += DT;

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
        m_isAppearComplete = true;
        cout << "Alpha Skill1 시퀸스 완료." << endl;
    }
}

void AlphaSkill1State::Exit(shared_ptr<ModelAnimator> _animator)
{
    if (!_animator)
        return;

    cout << "Alpha Skill1 상태 종료 - 대기 시간: " << m_animTime << "초" << endl;

    // 상태 종료 시 정리
    m_animTime = 0.0f;
    m_isAnimationStarted = false;
    m_isAppearComplete = false;
}

bool AlphaSkill1State::CanTransitionTo(AnimationStateType _nextState)
{
    // 스킬이 완료되었을 때만 Wait 상태로 전환 가능
    if (m_isAppearComplete && _nextState == AnimationStateType::Wait || _nextState == AnimationStateType::Move
        || _nextState == AnimationStateType::Dead)
    {
        return true;
    }
    return false;
}
