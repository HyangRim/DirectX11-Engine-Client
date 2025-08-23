#include "pch.h"
#include "PlayerStateMachine.h"

#include "NavMesh.h"
#include "NavMeshAgent.h"

#include "Camera.h"

#include "AnimationStateMachine.h"
#include "AnimationState.h"

#include "PlayerWaitState.h"
#include "PlayerRunState.h"

#include "PlayerQState.h"
#include "PlayerWState.h"
#include "PlayerEState.h"
#include "PlayerRState.h"

#include "BaseCollider.h"

#include "IPlayer.h"

PlayerStateMachine::PlayerStateMachine(shared_ptr<AnimationStateMachine> animationStateMachine, int chargingInfo, int isMovableOnSkill, int isNeedTarget)
    : Component(ComponentType::PlayerStateMachine) 
    , m_currentState(nullptr)
    , m_animationStateMachine(animationStateMachine)
    , m_chargingInfo(chargingInfo)
    , m_isMovableOnSkill(isMovableOnSkill)
    , m_isNeedTarget(isNeedTarget)
{
    // 이벤트 구독
    EVENT->Subscribe(EventType::PLAYER_STATE_CHANGE_REQUEST,
        [this](shared_ptr<EventData> eventData) {
            HandleStateChangeRequest(eventData);
        });
}

PlayerStateMachine::~PlayerStateMachine()
{
   
}

void PlayerStateMachine::Init()
{
    /*
    m_states[PlayerStateType::Wait] = make_shared<PlayerWaitState>();
    m_states[PlayerStateType::Run] = make_shared<PlayerRunState>();
    m_states[PlayerStateType::Skill_1] = make_shared<PlayerQState>(modelAnimator, m_chargingInfo & 8);
    m_states[PlayerStateType::Skill_2] = make_shared<PlayerWState>(modelAnimator);
    m_states[PlayerStateType::Skill_3] = make_shared<PlayerEState>(modelAnimator, m_chargingInfo & 2);
    m_states[PlayerStateType::Skill_4] = make_shared<PlayerRState>(modelAnimator);*/

    // 초기화 작업
    m_currentState = m_states[PlayerStateType::Wait];
}

void PlayerStateMachine::Start()
{
    Init();
}

void PlayerStateMachine::Update()
{
    PrintCurState();
    ProcessInput();
   
    if (m_currentState)
    {
        m_currentState->Update();

        HandleSpecialStateTransitions();
    }

    //// 공격을 위한 이동이 완료되었는지 체크
    //if (m_isMovingToAttack && m_attackTarget)
    //{
    //    auto navMeshAgent = GetGameObject()->GetNavMeshAgent();

    //    if (navMeshAgent->HasReachedDestination())
    //    {
    //        // 이동 완료, 공격 범위 내에 있는지 재확인
    //        Vec3 playerPos = GetGameObject()->GetTransform()->GetPosition();
    //        Vec3 targetPos = m_attackTarget->GetTransform()->GetPosition();
    //        float distance = Vec3::Distance(playerPos, targetPos);

    //        if (distance <= 3.0f) // 공격 범위
    //        {
    //            StartBaseAttack();
    //        }
    //        else
    //        {
    //            // 타겟이 이동했을 수도 있으므로 다시 추적
    //            MoveToAttackTarget(targetPos, 3.0f);
    //        }
    //    }
    //}
}

void PlayerStateMachine::OnDestroy()
{
    if (m_currentState)
    {
        m_currentState->Exit();
        m_currentState = nullptr;
    }
    m_states.clear();
}

void PlayerStateMachine::ChangeState(PlayerStateType newState)
{
    //=======================이벤트 매니저 이전 ===========================//
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
    //=======================이벤트 매니저 이전 ===========================//

    auto eventData = make_shared<PlayerStateChangeEventData>(
        EventType::PLAYER_STATE_CHANGE_REQUEST,
        GetGameObject(),
        newState);

    EVENT->QueueEvent(eventData);
}

bool PlayerStateMachine::CanChangeState(PlayerStateType newState)
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}

void PlayerStateMachine::ProcessInput()
{

    if ((GetCurrentState() == PlayerStateType::Skill_1) && ((1 << 3) & m_isMovableOnSkill))
    {
        if ((1 << 3) & m_chargingInfo)
        {
            if (!m_currentState->IsMovable())
            {
                // 이동 불가능한 상태 (Release 중)
                return;
            }
        }
        else
        {
            //cout << "Q 스킬 중 이동 금지\n";

            return;
        }
 
    }
    if ((GetCurrentState() == PlayerStateType::Skill_2) && ((1 << 2) & m_isMovableOnSkill))
    {
        //cout << "W 스킬 중 이동 금지\n";
        return;
    }
    if ((GetCurrentState() == PlayerStateType::Skill_3) && ((1 << 1) & m_isMovableOnSkill))
    {
        if ((1 << 1) & m_chargingInfo)
        {
            if (!m_currentState->IsMovable())
            {
                // 이동 불가능한 상태 (Release 중)
                return;
            }
        }
        else
        {
            //cout << "E 스킬 중 이동 금지\n";

            return;
        }
    }
    if ((GetCurrentState() == PlayerStateType::Skill_4) && ((1 << 0) & m_isMovableOnSkill))
    {
        //cout << "R 스킬 중 이동 금지\n";
        return;
    }
    if ((GetCurrentState() == PlayerStateType::Craft))
    {
        //cout << "W 스킬 중 이동 금지\n";
        return;
    }
    if (((GetCurrentState() == PlayerStateType::Counter)))
    {
        cout << "카운터 공격 중 이동금지\n";
        return;
    }



    // NavMeshAgent 가져오기 - 올바른 ComponentType 사용
    auto gameObject = GetGameObject();
    auto navMeshAgent = gameObject->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);

    if (!navMeshAgent)
    {
        cout << "NavMeshAgent not found!" << endl;
        return;
    }

    m_baseAttackDelayDuration += DT;
    // 우클릭 처리
    if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
    {
        // 우클릭으로 공격 대상 피킹
        auto attackTarget = CURSCENE->GetObjectManager()->PickObjectForAttack(GetGameObject());

        //cout << "BaseAttackDelay : " << m_baseAttackDelayDuration << endl;
        if (attackTarget && m_baseAttackDelay <= m_baseAttackDelayDuration)
        {
            m_baseAttackDelayDuration = 0.f;
            m_states[PlayerStateType::BaseAttack]->SetTarget(attackTarget);
            ChangeState(PlayerStateType::BaseAttack);
        }
        else if (attackTarget == nullptr)
        {
            m_states[PlayerStateType::BaseAttack]->SetTarget(nullptr);
            // 마우스 위치 유효성 검사
            POINT mousePos = INPUT->GetMousePos();
            if (mousePos.x < 0 || mousePos.y < 0) return;

            auto camera = CURSCENE->GetMainCamera();
            if (!camera) return;

            auto cameraComp = camera->GetCamera();
            if (!cameraComp) return;

            Ray ray = CreateRayFromMouse(mousePos, cameraComp);

            // NavMesh 찾기 및 Ray cast
            bool foundDestination = false;
            for (auto& obj : CURSCENE->GetObjects())
            {
                auto navMesh = obj->GetFixedComponent<NavMesh>(ComponentType::NavMesh);
                if (navMesh)
                {
                    Vec3 hitPoint;
                    if (navMesh->RaycastNavMesh(ray, hitPoint))
                    {
                        navMeshAgent->SetDestination(hitPoint);
                        foundDestination = true;

                        // 이동 명령이 성공하면 즉시 Run 상태로 전환
                        if (CanChangeState(PlayerStateType::Run))
                        {
                            ChangeState(PlayerStateType::Run);
                            m_animationStateMachine->ChangeState(AnimationStateType::Run);
                        }
                        break;
                    }
                }
            }

            if (!foundDestination)
            {
                cout << "No valid destination found on NavMesh" << endl;
            }
        }
    }

    // NavMeshAgent 상태 지속적 모니터링 (이동 완료 감지용)
    if (navMeshAgent->HasReachedDestination() && IsInState(PlayerStateType::Run))
    {

        ChangeState(PlayerStateType::Wait);
        m_animationStateMachine->ChangeState(AnimationStateType::Wait);
    }

    if (INPUT->GetButtonDown(KEY_TYPE::LCTRL) || INPUT->GetButton(KEY_TYPE::LCTRL))
    {
        cout << "왼쪽컨트롤 키 눌림\n";
        return;
    }

    // 스킬 입력 시 이동 중지
    if (INPUT->GetButtonDown(KEY_TYPE::Q))
    {
        
        float curCoolDown = m_playerInterface->GetCurSkillCooldown(0);
        float maxCoolDown = m_playerInterface->GetMaxSkillCooldown(0);

        cout << "최대 쿨타임 : " << maxCoolDown << ", 현재 쿨타임 : " << curCoolDown << endl;
        
        if (curCoolDown > 0.f) return;

        navMeshAgent->Stop(); // 이동 중지
        m_animationStateMachine->ChangeState(AnimationStateType::Skill_1);
        ChangeState(PlayerStateType::Skill_1);

        OnSkillUsed(0,nullptr);  // 0: Q 스킬 인덱스
    }
    if (INPUT->GetButtonDown(KEY_TYPE::W))
    {
        float curCoolDown = m_playerInterface->GetCurSkillCooldown(1);
        float maxCoolDown = m_playerInterface->GetMaxSkillCooldown(1);

        cout << "최대 쿨타임 : " << maxCoolDown << ", 현재 쿨타임 : " << curCoolDown << endl;

        if (curCoolDown > 0.f) return;

        navMeshAgent->Stop();
        m_animationStateMachine->ChangeState(AnimationStateType::Skill_2);
        ChangeState(PlayerStateType::Skill_2);

        OnSkillUsed(1, nullptr);  // 1: W 스킬 인덱스
    }
    if (INPUT->GetButtonDown(KEY_TYPE::E))
    {
        float curCoolDown = m_playerInterface->GetCurSkillCooldown(2);
        float maxCoolDown = m_playerInterface->GetMaxSkillCooldown(2);

        cout << "최대 쿨타임 : " << maxCoolDown << ", 현재 쿨타임 : " << curCoolDown << endl;

        if (curCoolDown > 0.f) return;

        navMeshAgent->Stop();
        m_animationStateMachine->ChangeState(AnimationStateType::Skill_3);
        ChangeState(PlayerStateType::Skill_3);

        OnSkillUsed(2, nullptr);  // 2: E 스킬 인덱스
    }
    if (INPUT->GetButtonDown(KEY_TYPE::R))
    {
        float curCoolDown = m_playerInterface->GetCurSkillCooldown(3);
        float maxCoolDown = m_playerInterface->GetMaxSkillCooldown(3);

        cout << "최대 쿨타임 : " << maxCoolDown << ", 현재 쿨타임 : " << curCoolDown << endl;

        if (curCoolDown > 0.f) return;

        if (m_isNeedTarget & (1 << 0))
        {
            // R 스킬은 타겟이 필요함
            if (CheckTargetForSkill(KEY_TYPE::R))
            {
                auto target = GetPickedTargetAtMouse();
                navMeshAgent->Stop();
                OnSkillUsed(3, target);  // R은 타겟 정보 전달
                m_animationStateMachine->ChangeState(AnimationStateType::Skill_4);
                ChangeState(PlayerStateType::Skill_4);

            }
            else
            {
                cout << "R 스킬: 유효한 타겟이 없습니다." << endl;
            }
        }
        else
        {
            navMeshAgent->Stop();
            m_animationStateMachine->ChangeState(AnimationStateType::Skill_4);
            ChangeState(PlayerStateType::Skill_4);

            OnSkillUsed(3, nullptr);  // 3: R 스킬 인덱스 
        }
    }
    // PlayerStateMachine.cpp - Z키 처리
    if (INPUT->GetButtonDown(KEY_TYPE::Z))
    {
        bool craftSuccess = false;
        OnTryCraftFirst(craftSuccess);  // Client에게 "첫 번째 조합 시도해줘" 요청
       
        if (!craftSuccess) return;

        // 애니메이션 전환
        auto navMeshAgent = GetGameObject()
            ->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);
        if (navMeshAgent)
            navMeshAgent->Stop();

        m_animationStateMachine->ChangeState(AnimationStateType::Craft);
        ChangeState(PlayerStateType::Craft);     
    }  
}

Ray PlayerStateMachine::CreateRayFromMouse(POINT mousePos, shared_ptr<Camera> camera)
{
    Viewport viewport = GRAPHICS->GetViewport();
    Matrix worldMatrix = Matrix::Identity;
    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjectionMatrix();

    // 화면 좌표를 NDC로 변환
    float x = (2.0f * mousePos.x) / viewport.GetWidth() - 1.0f;
    float y = 1.0f - (2.0f * mousePos.y) / viewport.GetHeight();

    // Near와 Far 평면의 월드 좌표 계산
    Vec3 nearPoint = viewport.UnProject(Vec3(mousePos.x, mousePos.y, 0.0f), worldMatrix, viewMatrix, projMatrix);
    Vec3 farPoint = viewport.UnProject(Vec3(mousePos.x, mousePos.y, 1.0f), worldMatrix, viewMatrix, projMatrix);

    Vec3 rayDirection = farPoint - nearPoint;
    rayDirection.Normalize();

    // Ray 방향이 아래쪽을 향하는지 확인
    if (rayDirection.y > 0) {
        cout << "WARNING: Ray pointing upward!" << endl;
    }

    return Ray(nearPoint, rayDirection);
}

bool PlayerStateMachine::IsInState(PlayerStateType state) const
{
    return GetCurrentState() == state;
}

void PlayerStateMachine::RegisterState(PlayerStateType type, shared_ptr<PlayerState> state)
{
    m_states[type] = state;
}

PlayerStateType PlayerStateMachine::GetCurrentState() const
{
    shared_ptr<PlayerState> m_tes = m_currentState;
    return m_currentState ? m_currentState->GetType() : PlayerStateType::Wait;
}

// 새로운 메서드 추가
void PlayerStateMachine::HandleSpecialStateTransitions()
{
    // Q 스킬 완료 후 Wait 상태로 전환
    if (GetCurrentState() == PlayerStateType::Skill_1)
    {
        if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            cout << "들어와라Q \n";
            auto gameObject = GetGameObject();
            auto navMeshAgent = gameObject->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);
            navMeshAgent->Stop();
            ChangeState(PlayerStateType::Wait);
            m_animationStateMachine->ChangeState(AnimationStateType::Wait);
        }
    }
    // W 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == PlayerStateType::Skill_2)
    {
        /*if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            ChangeState(PlayerStateType::Wait);
            m_animationStateMachine->ChangeState(AnimationStateType::Wait);
        }*/
    }
    // E 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == PlayerStateType::Skill_3)
    {
        if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            cout << "들어와라E \n";
            auto gameObject = GetGameObject();
            auto navMeshAgent = gameObject->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);
            navMeshAgent->Stop();
            ChangeState(PlayerStateType::Wait);
            m_animationStateMachine->ChangeState(AnimationStateType::Wait);
        }
    }
    // R 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == PlayerStateType::Skill_4)
    {
        if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            ChangeState(PlayerStateType::Wait);
            m_animationStateMachine->ChangeState(AnimationStateType::Wait);
        }
    }

    // Craft 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == PlayerStateType::Craft)
    {
        if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            ChangeState(PlayerStateType::Wait);
            m_animationStateMachine->ChangeState(AnimationStateType::Wait);
        }
    }

    else if (GetCurrentState() == PlayerStateType::Counter)
    {
        if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            cout << "Counter 상태 완료 - Wait로 전환" << endl;
            auto gameObject = GetGameObject();
            auto navMeshAgent = gameObject->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);
            navMeshAgent->Stop();
            ChangeState(PlayerStateType::Wait);
            m_animationStateMachine->ChangeState(AnimationStateType::Wait);
        }
    }
}

void PlayerStateMachine::PrintCurState()
{
    if (INPUT->GetButtonDown(KEY_TYPE::A))
    {
        switch (m_currentState->GetType())
        {
        case PlayerStateType::Skill_1:
            cout << "PlayerCurstate : Q 스킬 상태\n";
            break;
        case PlayerStateType::Skill_2:
            cout << "PlayerCurstate : W 스킬 상태\n";
            break;
        case PlayerStateType::Skill_3:
            cout << "PlayerCurstate : E 스킬 상태\n";
            break;
        case PlayerStateType::Skill_4:
            cout << "PlayerCurstate : R 스킬 상태\n";
            break;
        case PlayerStateType::Run:
            cout << "PlayerCurstate : Run 상태\n";
            break;
        case PlayerStateType::Wait:
            cout << "PlayerCurstate : Wait 상태\n";
            break;
        case PlayerStateType::Counter:
            cout << "PlayerCurstate : Counter 상태\n";
            break;
        case PlayerStateType::BaseAttack:
            cout << "PlayerCurstate : BaseAttack 상태\n";
            break;
        }
    }
}


// 새로 추가할 함수들 - 쿼드트리 피킹 시스템 활용
bool PlayerStateMachine::CheckTargetForSkill(KEY_TYPE skillKey)
{
    if (skillKey != KEY_TYPE::R) return true; // R이 아닌 스킬은 항상 허용

    POINT mousePos = INPUT->GetMousePos();
    if (mousePos.x < 0 || mousePos.y < 0) return false;

    auto camera = CURSCENE->GetMainCamera();
    if (!camera) return false;

    shared_ptr<Camera> cam = camera->GetCamera();
    if (!cam) return false;

    // SceneObjectManager의 쿼드트리 피킹 시스템 사용
    Ray ray = CURSCENE->GetObjectManager()->CreateRayFromScreen(Vec2(mousePos.x, mousePos.y), cam);

    // 쿼드트리에서 후보 객체들 가져오기
    auto quadTree = CURSCENE->GetQuadTree();
    if (!quadTree) return false;

    vector<shared_ptr<GameObject>> candidates = quadTree->Query(ray, cam);

    // 유효한 타겟이 있는지 확인
    for (auto& obj : candidates)
    {
        if (!obj->GetCollider()) continue;
        if (!obj->GetActive()) continue;

        // 자기 자신은 제외
        if (obj.get() == GetGameObject().get()) continue;

        // 화면 좌표 유효성 검사 (음수 좌표 제외)
        RECT objBounds = quadTree->GetObjectScreenBounds(obj, cam);
        int screenCenterX = (objBounds.left + objBounds.right) / 2;
        int screenCenterY = (objBounds.top + objBounds.bottom) / 2;

        if (screenCenterX < 0 || screenCenterY < 0) continue;

        // Ray 교차 검사
        float distance = 0.f;
        if (obj->GetCollider()->Intersects(ray, distance))
        {
            return true; // 피킹 가능한 대상 발견
        }
    }

    return false; // 피킹 가능한 대상 없음
}

shared_ptr<GameObject> PlayerStateMachine::GetPickedTargetAtMouse()
{
    POINT mousePos = INPUT->GetMousePos();
    auto camera = CURSCENE->GetMainCamera();
    if (!camera) return nullptr;

    shared_ptr<Camera> cam = camera->GetCamera();
    if (!cam) return nullptr;

    // SceneObjectManager의 쿼드트리 피킹 시스템 사용
    Ray ray = CURSCENE->GetObjectManager()->CreateRayFromScreen(Vec2(mousePos.x, mousePos.y), cam);

    auto quadTree = CURSCENE->GetQuadTree();
    if (!quadTree) return nullptr;

    vector<shared_ptr<GameObject>> candidates = quadTree->Query(ray, cam);

    float minDistance = FLT_MAX;
    shared_ptr<GameObject> closestTarget = nullptr;

    for (auto& obj : candidates)
    {
        if (!obj->GetCollider()) continue;
        if (!obj->GetActive()) continue;

        // 자기 자신은 제외
        if (obj.get() == GetGameObject().get()) continue;

        // 화면 좌표 유효성 검사 (음수 좌표 제외)
        RECT objBounds = quadTree->GetObjectScreenBounds(obj, cam);
        int screenCenterX = (objBounds.left + objBounds.right) / 2;
        int screenCenterY = (objBounds.top + objBounds.bottom) / 2;

        if (screenCenterX < 0 || screenCenterY < 0) continue;

        // Ray 교차 검사
        float distance = 0.f;
        if (obj->GetCollider()->Intersects(ray, distance))
        {
            if (distance < minDistance)
            {
                minDistance = distance;
                closestTarget = obj;
            }
        }
    }

    return closestTarget;
}

bool PlayerStateMachine::IsSkillOnCooldown(int skillIndex)
{
    bool isOnCooldown = false;
    OnSkillCooldownCheck(skillIndex, isOnCooldown);
    return isOnCooldown;
}

void PlayerStateMachine::SetAttackTarget(shared_ptr<GameObject> target)
{
    m_attackTarget = target;
    m_isMovingToAttack = true;
}

void PlayerStateMachine::MoveToAttackTarget(Vec3 targetPos, float attackRange)
{
    auto navMeshAgent = GetGameObject()->GetNavMeshAgent();
    if (!navMeshAgent) return;

    // 타겟 주변의 공격 가능한 위치 계산
    Vec3 playerPos = GetGameObject()->GetTransform()->GetPosition();
    Vec3 direction = playerPos - targetPos;
    direction.Normalize();

    // 공격 범위보다 약간 가까운 거리로 설정
    Vec3 attackPosition = targetPos + direction * (attackRange - 0.5f);

    navMeshAgent->SetDestination(attackPosition);

    // Run 상태로 전환
    if (CanChangeState(PlayerStateType::Run))
    {
        ChangeState(PlayerStateType::Run);
        m_animationStateMachine->ChangeState(AnimationStateType::Run);
    }
}

void PlayerStateMachine::StartBaseAttack()
{
    auto navMeshAgent = GetGameObject()->GetNavMeshAgent();
    if (navMeshAgent) navMeshAgent->Stop();

    // 평타 상태로 전환
    ChangeState(PlayerStateType::BaseAttack);
    m_animationStateMachine->ChangeState(AnimationStateType::BaseAttack);

    m_isMovingToAttack = false;
}


void PlayerStateMachine::HandleStateChangeRequest(shared_ptr<EventData> eventData)
{
    auto stateChangeData = static_pointer_cast<PlayerStateChangeEventData>(eventData);

    if (stateChangeData->m_target != GetGameObject())
        return;

    ChangeStateImmediate(stateChangeData->m_newState);
}

void PlayerStateMachine::ChangeStateImmediate(PlayerStateType newState)
{
    if (!CanChangeState(newState))
        return;

    PlayerStateType oldState = GetCurrentState();

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
        EventType::PLAYER_STATE_CHANGED,
        GetGameObject(),
        static_cast<int>(oldState),
        static_cast<int>(newState)
    );
    EVENT->TriggerEvent(completedEventData);
}