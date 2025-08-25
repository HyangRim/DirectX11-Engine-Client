// MonsterStateMachine.cpp
#include "pch.h"
#include "MonsterStateMachine.h"
#include "GameObject.h"
#include "AnimationStateMachine.h"
#include "NavMeshAgent.h"

MonsterStateMachine::MonsterStateMachine()
    : Super(ComponentType::MonsterStateMachine)
{
    // 몬스터 상태 변경 요청 이벤트 구독
    EVENT->Subscribe(EventType::MONSTER_STATE_CHANGE_REQUEST,
        [this](shared_ptr<EventData> eventData) {
            HandleStateChangeRequest(eventData);
        });

    // 애니메이션 상태 변경 완료 이벤트 구독 (연동)
    EVENT->Subscribe(EventType::ANIMATION_STATE_CHANGED,
        [this](shared_ptr<EventData> eventData) {
            HandleAnimationStateChanged(eventData);
        });
}

MonsterStateMachine::~MonsterStateMachine()
{
    EVENT->UnsubscribeAll(EventType::MONSTER_STATE_CHANGE_REQUEST);
    EVENT->UnsubscribeAll(EventType::ANIMATION_STATE_CHANGED);
}

void MonsterStateMachine::Start()
{
    Super::Start();

    // 컴포넌트 참조 가져오기
    auto gameObject = GetGameObject();
    if (gameObject)
    {
        m_animationStateMachine = gameObject->GetAnimationStateMachine();
        m_navMeshAgent = gameObject->GetNavMeshAgent();
    }

    // 초기 상태 설정 (Appear 또는 Wait)
    if (m_states.find(MonsterStateType::Appear) != m_states.end())
    {
        m_currentState = m_states[MonsterStateType::Appear];
        if (m_currentState)
        {
            m_currentState->Enter();
        }
    }
}

void MonsterStateMachine::Update()
{
    Super::Update();

    // AI 로직 처리
    ProcessAI();

    // 대기열의 상태 변경 요청 처리
    while (!m_stateChangeQueue.empty())
    {
        MonsterStateType newState = m_stateChangeQueue.front();
        m_stateChangeQueue.pop();
        ExecuteStateChange(newState);
    }

    // 현재 상태 업데이트
    if (m_currentState)
    {
        m_currentState->Update();
    }
}

void MonsterStateMachine::OnDestroy()
{
    if (m_currentState)
    {
        m_currentState->Exit();
    }
    m_currentState = nullptr;
    m_states.clear();

    // 이벤트 구독 해제
    EVENT->UnsubscribeAll(EventType::MONSTER_STATE_CHANGE_REQUEST);
    EVENT->UnsubscribeAll(EventType::ANIMATION_STATE_CHANGED);

    Super::OnDestroy();
}

void MonsterStateMachine::RequestStateChange(MonsterStateType newState)
{
    // EventManager를 통해 상태 변경 요청
    auto eventData = make_shared<MonsterStateChangeEventData>(
        EventType::MONSTER_STATE_CHANGE_REQUEST,
        GetGameObject(),
        newState
    );
    EVENT->QueueEvent(eventData);
}

void MonsterStateMachine::ExecuteStateChange(MonsterStateType newState)
{
    if (!CanChangeState(newState))
        return;

    MonsterStateType oldState = GetCurrentState();

    // 현재 상태 종료
    if (m_currentState)
    {
        m_currentState->Exit();
    }

    // 새 상태 시작
    m_currentState = m_states[newState];
    if (m_currentState)
    {
        m_currentState->Enter();
    }

    // 상태 변경 완료 이벤트 발생
    auto completedEventData = make_shared<StateEventData>(
        EventType::MONSTER_STATE_CHANGED,
        GetGameObject(),
        static_cast<int>(oldState),
        static_cast<int>(newState)
    );
    EVENT->TriggerEvent(completedEventData);

    if (m_enableDebugLog)
    {
        cout << "MonsterState changed: " << static_cast<int>(oldState)
            << " -> " << static_cast<int>(newState) << endl;
    }
}

void MonsterStateMachine::ProcessAI()
{
    // 타겟 감지 업데이트
    UpdateTargetDetection();

    // 현재 상태에 따른 AI 로직
    MonsterStateType currentState = GetCurrentState();

    switch (currentState)
    {
    case MonsterStateType::Wait:
        if (m_target)
        {
            RequestStateChange(MonsterStateType::Trace);
        }
        break;

    case MonsterStateType::Trace:
        if (!m_target)
        {
            RequestStateChange(MonsterStateType::Wait);
        }
        else
        {
            // 타겟과의 거리 체크
            float distance = Vec3::Distance(
                GetGameObject()->GetTransform()->GetPosition(),
                m_target->GetTransform()->GetPosition()
            );

            if (distance <= m_attackRange)
            {
                RequestStateChange(MonsterStateType::Attack);
            }
        }
        break;

    case MonsterStateType::Attack:
        // 공격 완료 후 로직은 애니메이션 완료 이벤트로 처리
        break;
    }
}

void MonsterStateMachine::UpdateTargetDetection()
{
    // 플레이어 탐지 로직
    auto gameObjects = CURSCENE->GetObjects();

    for (auto& obj : gameObjects)
    {
        if (obj->GetType() == OBJECTTYPE::PLAYER)
        {
            float distance = Vec3::Distance(
                GetGameObject()->GetTransform()->GetPosition(),
                obj->GetTransform()->GetPosition()
            );

            if (distance <= m_detectionRange)
            {
                SetTarget(obj);
                return;
            }
        }
    }

    // 범위 내에 플레이어가 없으면 타겟 해제
    if (m_target)
    {
        float distance = Vec3::Distance(
            GetGameObject()->GetTransform()->GetPosition(),
            m_target->GetTransform()->GetPosition()
        );

        if (distance > m_detectionRange * 1.5f) // 히스테리시스 적용
        {
            SetTarget(nullptr);
        }
    }
}

void MonsterStateMachine::HandleStateChangeRequest(shared_ptr<EventData> eventData)
{
    auto stateChangeData = static_pointer_cast<MonsterStateChangeEventData>(eventData);

    // 자신의 GameObject인지 확인
    if (stateChangeData->m_target != GetGameObject())
        return;

    // 상태 변경 대기열에 추가
    m_stateChangeQueue.push(stateChangeData->m_newState);
}

void MonsterStateMachine::HandleAnimationStateChanged(shared_ptr<EventData> eventData)
{
    auto stateData = static_pointer_cast<StateEventData>(eventData);

    // 자신의 GameObject인지 확인
    if (stateData->m_owner != GetGameObject())
        return;

    // 애니메이션 상태 변경에 따른 몬스터 상태 자동 전환 로직
    AnimationStateType animState = static_cast<AnimationStateType>(stateData->m_toState);

    switch (animState)
    {
    case AnimationStateType::Wait:
        if (IsInState(MonsterStateType::Attack))
        {
            // 공격 완료 후 추적 또는 대기 상태로 복귀
            if (m_target)
            {
                RequestStateChange(MonsterStateType::Trace);
            }
            else
            {
                RequestStateChange(MonsterStateType::Wait);
            }
        }
        break;
    }
}

bool MonsterStateMachine::CanChangeState(MonsterStateType newState) const
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}

MonsterStateType MonsterStateMachine::GetCurrentState() const
{
    return m_currentState ? m_currentState->GetType() : MonsterStateType::Wait;
}

shared_ptr<MonsterState> MonsterStateMachine::GetCurrentStatePtr() const
{
    return m_currentState;
}

bool MonsterStateMachine::IsInState(MonsterStateType state) const
{
    return GetCurrentState() == state;
}

void MonsterStateMachine::RegisterState(MonsterStateType type, shared_ptr<MonsterState> state)
{
    m_states[type] = state;
}

void MonsterStateMachine::SetTarget(shared_ptr<GameObject> target)
{
    m_target = target;
}

shared_ptr<GameObject> MonsterStateMachine::GetTarget() const
{
    return m_target;
}
