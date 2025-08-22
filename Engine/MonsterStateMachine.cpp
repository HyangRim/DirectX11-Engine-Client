#include "pch.h"
#include "MonsterStateMachine.h"
#include "AnimationStateMachine.h"

MonsterStateMachine::MonsterStateMachine(shared_ptr<AnimationStateMachine> animationStateMachine)
	: Component(ComponentType::MonsterStateMachine)
    , m_animationStateMachine(animationStateMachine)
{
    // 이벤트 구독
    EVENT->Subscribe(EventType::MONSTER_STATE_CHANGE_REQUEST,
        [this](shared_ptr<EventData> eventData) {
            HandleStateChangeRequest(eventData);
        });
}

MonsterStateMachine::~MonsterStateMachine()
{
}

void MonsterStateMachine::Start()
{
    m_currentState = m_states[MonsterStateType::Appear];
    if (m_currentState)
    {
        m_currentState->Enter();
    }
}

void MonsterStateMachine::Update()
{
    ProcessInput();
    PrintCurState();
    if (m_currentState)
    {
        m_currentState->Update();
        HandleSpecialStateTransitions();
    }
}


void MonsterStateMachine::PrintCurState()
{
    if (GetGameObject()->GetName().compare(L"Alpha") != 0)
        return;

    if (INPUT->GetButtonDown(KEY_TYPE::A))
    {
        switch (m_currentState->GetType())
        {
        case MonsterStateType::Move:
            cout << "MonsterCurState : Run 상태\n";
            break;
        case MonsterStateType::Wait:
            cout << "MonsterCurState : Wait 상태\n";
            break;
        case MonsterStateType::Trace:
            cout << "MonsterCurState : Trace 상태\n";
            break;
        case MonsterStateType::Attack:
            cout << "MonsterCurState : 기본공격 상태\n";
            break;
        }
    }
}

void MonsterStateMachine::ChangeState(MonsterStateType newState)
{
    //if (!CanChangeState(newState))
    //    return;

    //// 현재 상태 종료
    //if (m_currentState)
    //{
    //    m_currentState->Exit();
    //}

    //// 새 상태 시작
    //m_currentState = m_states[newState];
    //if (m_currentState)
    //{
    //    m_currentState->Enter();
    //}


    auto eventData = make_shared<MonsterStateChangeEventData>(
        EventType::MONSTER_STATE_CHANGE_REQUEST,
        GetGameObject(),
        newState);

    EVENT->QueueEvent(eventData);
}

bool MonsterStateMachine::CanChangeState(MonsterStateType newState)
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}

MonsterStateType MonsterStateMachine::GetCurrentState() const
{
    return m_currentState ? m_currentState->GetType() : MonsterStateType::Wait;
}

void MonsterStateMachine::RegisterState(MonsterStateType type, shared_ptr<MonsterState> state)
{
    m_states[type] = state;
}

void MonsterStateMachine::ProcessInput()
{
    if (INPUT->GetButtonDown(KEY_TYPE::KEY_5)) {
        ChangeState(MonsterStateType::Appear);
        m_animationStateMachine->ChangeState(AnimationStateType::Appear);
    }
    else if (INPUT->GetButtonDown(KEY_TYPE::KEY_6)) {
        ChangeState(MonsterStateType::Wait);
        m_animationStateMachine->ChangeState(AnimationStateType::Wait);
    }
    else if (INPUT->GetButtonDown(KEY_TYPE::KEY_7)) {
  
    }
    else if (INPUT->GetButtonDown(KEY_TYPE::KEY_8)) {
        ChangeState(MonsterStateType::Death);
        m_animationStateMachine->ChangeState(AnimationStateType::Death);
    }
}

// MonsterStateMachine.cpp의 HandleSpecialStateTransitions() 추가
void MonsterStateMachine::HandleSpecialStateTransitions()
{
    // Appear 상태 완료 후 Wait 상태로 전환
    if (GetCurrentState() == MonsterStateType::Appear)
    {
        // CanTransitionTo로 전환 가능 여부 체크
        if (m_currentState && m_currentState->CanTransitionTo(MonsterStateType::Wait))
        {
            cout << "Appear 상태 완료 조건 만족 - Wait로 전환" << endl;
            ChangeState(MonsterStateType::Wait);
            m_animationStateMachine->ChangeState(AnimationStateType::Wait);
        }
    }
    else if (GetCurrentState() == MonsterStateType::Death)
    {
        // CanTransitionTo로 전환 가능 여부 체크
        // 어차피 LogicState에서 완료를 판정했으므로 전환해도 무방
        if (m_currentState && m_currentState->CanTransitionTo(MonsterStateType::Dying))
        {
            cout << "Death 상태 완료 조건 만족 - Dying로 전환" << endl;
            ChangeState(MonsterStateType::Dying);
            m_animationStateMachine->ChangeState(AnimationStateType::Dying);
        }
    }
    //else if (GetCurrentState() == MonsterStateType::Attack)
    //{
    //    // CanTransitionTo로 전환 가능 여부 체크
    //    // 어차피 LogicState에서 완료를 판정했으므로 전환해도 무방
    //    if (m_currentState && m_currentState->CanTransitionTo(MonsterStateType::Wait))
    //    {
    //        cout << "Attack 상태 완료 조건 만족 - Wait로 전환" << endl;
    //        ChangeState(MonsterStateType::Wait);
    //        m_animationStateMachine->ChangeState(AnimationStateType::Wait);
    //    }
    //}
}



void MonsterStateMachine::HandleStateChangeRequest(shared_ptr<EventData> eventData)
{
    auto stateChangeData = static_pointer_cast<MonsterStateChangeEventData>(eventData);

    if (stateChangeData->m_target != GetGameObject())
        return;

    ChangeStateImmediate(stateChangeData->m_newState);
}

void MonsterStateMachine::ChangeStateImmediate(MonsterStateType newState)
{
    if (!CanChangeState(newState))
        return;

    MonsterStateType oldState = GetCurrentState();

    if (m_currentState)
    {
        m_currentState->Exit();
    }

    m_currentState = m_states[newState];
    if (m_currentState)
    {
        m_currentState->Enter();
    }

    // 상태 변경 완료 이벤트
    auto completedEventData = make_shared<StateEventData>(
        EventType::MONSTER_STATE_CHANGED,
        GetGameObject(),
        static_cast<int>(oldState),
        static_cast<int>(newState)
    );
    EVENT->TriggerEvent(completedEventData);
}