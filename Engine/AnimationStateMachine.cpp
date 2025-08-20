// AnimationStateMachine.cpp
#include "pch.h"
#include "AnimationStateMachine.h"
#include "GameObject.h"
#include "ModelAnimator.h"
#include "NavMesh.h"
#include "NavMeshAgent.h"
#include "Camera.h"
#include "Viewport.h"

#include "PlayerStateMachine.h"


// ... 다른 상태들 include

AnimationStateMachine::AnimationStateMachine(AnimationStateType initialState)
    : Super(ComponentType::AnimationStateMachine)
{
    m_initialStateType = initialState;

    // 이벤트 구독
    EVENT->Subscribe(EventType::ANIMATION_STATE_CHANGE_REQUEST,
        [this](shared_ptr<EventData> eventData) {
            HandleStateChangeRequest(eventData);
        });
}

AnimationStateMachine::~AnimationStateMachine()
{
}

void AnimationStateMachine::Start()
{
    Super::Start();

    auto gameObject = GetGameObject();
    if (gameObject)
    {
        m_animator = gameObject->GetModelAnimator();
    }

    InitializeStates();

    // 초기 상태 설정
    m_currentState = m_states[m_initialStateType];
    if (m_currentState)
    {
        m_currentState->Enter(m_animator);
    }
}

void AnimationStateMachine::Update()
{
    Super::Update();
    PrintCurState();
   // ProcessInput();

    if (m_currentState)
    {
        m_currentState->Update(m_animator);     

        // 특수 상태 처리 추가
        HandleSpecialStateTransitions();
    }
}

void AnimationStateMachine::PrintCurState()
{
    if (INPUT->GetButtonDown(KEY_TYPE::A))
    {
        switch (m_currentState->GetType())
        {
        case AnimationStateType::Skill_1:
            cout << "AnimationCurState : Q 스킬 상태\n";
            break;
        case AnimationStateType::Skill_2:
            cout << "AnimationCurState : W 스킬 상태\n";
            break;
        case AnimationStateType::Skill_3:
            cout << "AnimationCurState : E 스킬 상태\n";
            break;
        case AnimationStateType::Skill_4:
            cout << "AnimationCurState : R 스킬 상태\n";
            break;
        case AnimationStateType::Move:
            cout << "AnimationCurState : Run 상태\n";
            break;
        case AnimationStateType::Wait:
            cout << "AnimationCurState : Wait 상태\n";
            break;
        } 
    }
}

void AnimationStateMachine::ChangeState(AnimationStateType newState)
{
    // ============== 이벤트 매니저 이전 ================ //
    //if (!CanChangeState(newState))
    //    return;

    //if (m_animator == nullptr) {
    //    auto gameObject = GetGameObject();
    //    if (gameObject)
    //    {
    //        m_animator = gameObject->GetModelAnimator();
    //    }
    //}

    //// 현재 상태 종료
    //if (m_currentState)
    //{
    //    m_currentState->Exit(m_animator);
    //}

    //// 새 상태 시작
    //m_currentState = m_states[newState];
    //if (m_currentState)
    //{
    //    m_currentState->Enter(m_animator);
    //}
    // ============== 이벤트 매니저 이전 ================ //

    auto eventData = make_shared<AnimationStateChangeEventData>(
        EventType::ANIMATION_STATE_CHANGE_REQUEST,
        GetGameObject(),
        newState);

    EVENT->QueueEvent(eventData);
}

bool AnimationStateMachine::CanChangeState(AnimationStateType newState)
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}
void AnimationStateMachine::SetInitialState(AnimationStateType initialState)
{
    m_initialStateType = initialState;
}


AnimationStateType AnimationStateMachine::GetCurrentState() const
{
    return m_currentState ? m_currentState->GetType() : AnimationStateType::Wait;
}


shared_ptr<AnimationState> AnimationStateMachine::GetState(AnimationStateType type)
{
    return  m_states[type];
}

bool AnimationStateMachine::IsInState(AnimationStateType state) const
{
    return GetCurrentState() == state;
}

void AnimationStateMachine::InitializeStates()
{
    // ... 다른 상태들 등록
}
//
// 새로운 메서드 추가
void AnimationStateMachine::HandleSpecialStateTransitions()
{
    // Q 스킬 완료 후 Wait 상태로 전환
    if (GetCurrentState() == AnimationStateType::Skill_1)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }
    // W 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == AnimationStateType::Skill_2)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }
    // E 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == AnimationStateType::Skill_3)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }
    // R 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == AnimationStateType::Skill_4)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }

    // R 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == AnimationStateType::Craft)
    {
        if (m_currentState->CanTransitionTo(AnimationStateType::Wait))
        {
            ChangeState(AnimationStateType::Wait);
        }
    }

    // 다른 스킬들도 동일하게 처리 가능
    // if (GetCurrentState() == AnimationStateType::Skill_1) { ... }
}

void AnimationStateMachine::RegisterState(AnimationStateType type, shared_ptr<AnimationState> state)
{
    m_states[type] = state;
}


void AnimationStateMachine::HandleStateChangeRequest(shared_ptr<EventData> eventData)
{
    auto stateChangeData = static_pointer_cast<AnimationStateChangeEventData>(eventData);

    // 자신의 GameObject인지 확인
    if (stateChangeData->m_target != GetGameObject())
        return;

    ChangeStateImmediate(stateChangeData->m_newState);
}

void AnimationStateMachine::ChangeStateImmediate(AnimationStateType newState)
{
    if (!CanChangeState(newState))
        return;

    AnimationStateType oldState = GetCurrentState();

    // 현재 상태 종료
    if (m_currentState)
    {
        m_currentState->Exit(m_animator);
    }

    // 새 상태 시작
    m_currentState = m_states[newState];
    if (m_currentState)
    {
        m_currentState->Enter(m_animator);
    }

    // 상태 변경 완료 이벤트 발생
    auto completedEventData = make_shared<StateEventData>(
        EventType::ANIMATION_STATE_CHANGED,
        GetGameObject(),
        static_cast<int>(oldState),
        static_cast<int>(newState)
    );
    EVENT->TriggerEvent(completedEventData);
}