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

#include "SkillConfig.h"

PlayerStateMachine::PlayerStateMachine(uint32 _characterIdx)
    : Component(ComponentType::PlayerStateMachine) 
    , m_characterIndex(_characterIdx)
{
    // 플레이어 상태 변경 요청 이벤트 구독
    EVENT->Subscribe(EventType::PLAYER_STATE_CHANGE_REQUEST,
        [this](shared_ptr<EventData> eventData) {
            HandleStateChangeRequest(eventData);
        });

    // 애니메이션 상태 변경 완료 이벤트 구독 (연동)
    EVENT->Subscribe(EventType::ANIMATION_STATE_CHANGED,
        [this](shared_ptr<EventData> eventData) {
            HandleAnimationStateChanged(eventData);
        });
}

PlayerStateMachine::~PlayerStateMachine()
{
    EVENT->UnsubscribeAll(EventType::PLAYER_STATE_CHANGE_REQUEST);
    EVENT->UnsubscribeAll(EventType::ANIMATION_STATE_CHANGED);
}

void PlayerStateMachine::Start()
{
    Super::Start();

    // 컴포넌트 참조 가져오기
    auto gameObject = GetGameObject();
    if (gameObject)
    {
        m_animationStateMachine = gameObject->GetAnimationStateMachine();
        m_navMeshAgent = gameObject->GetNavMeshAgent();
    }

    // 초기 상태 설정 (Wait)
    if (m_states.find(PlayerStateType::Wait) != m_states.end())
    {
        m_currentState = m_states[PlayerStateType::Wait];
        if (m_currentState)
        {
            m_currentState->Enter();
        }
    }
}

void PlayerStateMachine::Update()
{
    Super::Update();

    PrintCurState();

    // 입력 처리
    if (m_inputEnabled)
    {
        ProcessInput();
    }
   
    //이동 완료 감지
    CheckMovementCompletion();

    //제작 완료 감지
    CheckCraftCompletion();

    //Q스킬 종료 감지
    CheckQSkillCompletion();
    CheckWSkillCompletion();
    CheckESkillCompletion();
    CheckRSkillCompletion();


    // 대기열의 상태 변경 요청 처리
    while (!m_stateChangeQueue.empty())
    {
        PlayerStateType newState = m_stateChangeQueue.front();
        m_stateChangeQueue.pop();
        ExecuteStateChange(newState);
    }

    // 현재 상태 업데이트
    if (m_currentState)
    {
        m_currentState->Update();
    }
}

void PlayerStateMachine::OnDestroy()
{
    if (m_currentState)
    {
        m_currentState->Exit();
    }
    m_currentState = nullptr;
    m_states.clear();

    // 이벤트 구독 해제
    EVENT->UnsubscribeAll(EventType::PLAYER_STATE_CHANGE_REQUEST);
    EVENT->UnsubscribeAll(EventType::ANIMATION_STATE_CHANGED);

    Super::OnDestroy();
}


void PlayerStateMachine::RequestStateChange(PlayerStateType newState)
{
    // EventManager를 통해 상태 변경 요청
    auto eventData = make_shared<PlayerStateChangeEventData>(
        EventType::PLAYER_STATE_CHANGE_REQUEST,
        GetGameObject(),
        newState
    );
    EVENT->QueueEvent(eventData);
}

bool PlayerStateMachine::CanChangeState(PlayerStateType newState)
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}


PlayerStateType PlayerStateMachine::GetCurrentState() const
{
    return m_currentState ? m_currentState->GetType() : PlayerStateType::Wait;
}

shared_ptr<PlayerState> PlayerStateMachine::GetCurrentStatePtr() const
{
    return m_currentState;
}

shared_ptr<PlayerState> PlayerStateMachine::GetState(PlayerStateType type) const
{
    auto it = m_states.find(type);
    if (it != m_states.end()) {
        return it->second;  // 찾은 경우 값 반환
    }
    return nullptr;  // 찾지 못한 경우 nullptr 반환
}

bool PlayerStateMachine::IsInState(PlayerStateType state) const
{
    return GetCurrentState() == state;
}


void PlayerStateMachine::RegisterState(PlayerStateType type, shared_ptr<PlayerState> state)
{
    m_states[type] = state;
}


void PlayerStateMachine::ExecuteStateChange(PlayerStateType newState)
{
    if (!CanChangeState(newState))
        return;

    PlayerStateType oldState = GetCurrentState();

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
        EventType::PLAYER_STATE_CHANGED,
        GetGameObject(),
        static_cast<int>(oldState),
        static_cast<int>(newState)
    );
    EVENT->TriggerEvent(completedEventData);

    if (m_enableDebugLog)
    {
        cout << "PlayerState changed: " << static_cast<int>(oldState)
            << " -> " << static_cast<int>(newState) << endl;
    }
}


void PlayerStateMachine::ProcessInput()
{ 
    // 스킬 입력 처리
    HandleSkillInput();

    // 제작 입력 처리
    HandleCraftInput();





    //// 공격 입력 처리
    //HandleAttackInput();

    //// 이동 입력 처리
    //HandleMovementInput();


    // 3. 우클릭 처리 (공격 vs 이동 판단)
    HandleRightClickInput(); // 새로운 통합 함수
    

   
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



void PlayerStateMachine::SetPlayerInterface(shared_ptr<IPlayer> playerInterface)
{
    m_playerInterface = playerInterface;
}

void PlayerStateMachine::SetAttackTarget(shared_ptr<GameObject> target)
{
    m_attackTarget = target;
    m_isMovingToAttack = true;
}

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



void PlayerStateMachine::HandleStateChangeRequest(shared_ptr<EventData> eventData)
{
    /*auto stateChangeData = static_pointer_cast<PlayerStateChangeEventData>(eventData);

    if (stateChangeData->m_target != GetGameObject())
        return;

    ChangeStateImmediate(stateChangeData->m_newState);*/

    auto stateChangeData = static_pointer_cast<PlayerStateChangeEventData>(eventData);

    // 자신의 GameObject인지 확인
    if (stateChangeData->m_target != GetGameObject())
        return;

    // 상태 변경 대기열에 추가
    m_stateChangeQueue.push(stateChangeData->m_newState);
}

void PlayerStateMachine::HandleAnimationStateChanged(shared_ptr<EventData> eventData)
{
    auto stateData = static_pointer_cast<StateEventData>(eventData);

    // 자신의 GameObject인지 확인
    if (stateData->m_owner != GetGameObject())
        return;

    // 애니메이션 상태 변경에 따른 플레이어 상태 자동 전환 로직
    AnimationStateType animState = static_cast<AnimationStateType>(stateData->m_toState);

    // 예: 스킬 애니메이션이 완료되면 Wait 상태로 전환
    switch (animState)
    {
    case AnimationStateType::Wait:
        if (IsInState(PlayerStateType::Skill_1) ||
            IsInState(PlayerStateType::Skill_2) ||
            IsInState(PlayerStateType::Skill_3) ||
            IsInState(PlayerStateType::Skill_4))
        {
            RequestStateChange(PlayerStateType::Wait);
        }
        break;
    }
}

void PlayerStateMachine::HandleMovementInput()
{
    //if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
    //{
    //    //이동은 Wait나 Run 상태일때만
    //    if (!(IsInState(PlayerStateType::Run) == true || IsInState(PlayerStateType::Wait) == true))
    //    {
    //        return;
    //    }

    //    // 우클릭 이동 로직
    //    // NavMeshAgent를 통한 이동 처리
    //    if (m_navMeshAgent)
    //    {
    //        //마우스 위치 유효성 검사
    //        POINT mousePos = INPUT->GetMousePos();
    //        if (mousePos.x < 0 || mousePos.y < 0) return;
 
    //        auto camera = CURSCENE->GetMainCamera();
    //        if (!camera) return;
 
    //        auto cameraComp = camera->GetCamera();
    //        if (!cameraComp) return;
 
    //        Ray ray = CreateRayFromMouse(mousePos, cameraComp);
 
    //        // NavMesh 찾기 및 Ray cast
    //        bool foundDestination = false;
    //        for (auto& obj : CURSCENE->GetObjects())
    //        {
    //            auto navMesh = obj->GetFixedComponent<NavMesh>(ComponentType::NavMesh);
    //            if (navMesh)
    //            {
    //                Vec3 hitPoint;
    //                if (navMesh->RaycastNavMesh(ray, hitPoint))
    //                {
    //                    GetGameObject()->GetNavMeshAgent()->SetDestination(hitPoint);
    //                    foundDestination = true;
 
    //                    // 이동 명령이 성공하면 즉시 Run 상태로 전환
    //                    cout << "이동 시작 - Run 상태로 전환" << endl;
    //                    RequestStateChange(PlayerStateType::Run);
    //                    // 애니메이션도 동시에 전환
    //                    if (m_animationStateMachine)
    //                    {
    //                        m_animationStateMachine->RequestStateChange(AnimationStateType::Run);
    //                    }
    //                    break;
    //                }
    //            }
    //        }
    //        if (!foundDestination)
    //        {
    //            cout << "No valid destination found on NavMesh" << endl;
    //        }
    //    }
    //}



    if (m_navMeshAgent)
    {
        POINT mousePos = INPUT->GetMousePos();
        if (mousePos.x < 0 || mousePos.y < 0) return;

        auto camera = CURSCENE->GetMainCamera();
        if (!camera) return;

        auto cameraComp = camera->GetCamera();
        if (!cameraComp) return;

        Ray ray = CreateRayFromMouse(mousePos, cameraComp);

        // NavMesh 이동 로직...
        bool foundDestination = false;
        for (auto& obj : CURSCENE->GetObjects())
        {
            auto navMesh = obj->GetFixedComponent<NavMesh>(ComponentType::NavMesh);
            if (navMesh)
            {
                Vec3 hitPoint;
                if (navMesh->RaycastNavMesh(ray, hitPoint))
                {
                    GetGameObject()->GetNavMeshAgent()->SetDestination(hitPoint);
                    foundDestination = true;

                    cout << "이동 시작 - Run 상태로 전환" << endl;
                    RequestStateChange(PlayerStateType::Run);

                    if (m_animationStateMachine)
                    {
                        m_animationStateMachine->RequestStateChange(AnimationStateType::Run);
                    }
                    break;
                }
            }
        }

        if (!foundDestination)
        {
            cout << "유효한 이동 지점을 찾을 수 없습니다" << endl;
        }
    }
}

void PlayerStateMachine::CheckMovementCompletion()
{
    // Run 상태일 때만 도착 체크
    if (IsInState(PlayerStateType::Run) && m_navMeshAgent)
    {
        if (m_navMeshAgent->HasReachedDestination())
        {
            cout << "목적지 도착 - Wait 상태로 전환" << endl;

            // Player 상태를 Wait로 변경
            RequestStateChange(PlayerStateType::Wait);

            // 애니메이션도 Wait로 변경
            if (m_animationStateMachine)
            {
                m_animationStateMachine->RequestStateChange(AnimationStateType::Wait);
            }
        }
    }
}

void PlayerStateMachine::HandleSkillInput()
{
    // Q 스킬
    if (INPUT->GetButtonDown(KEY_TYPE::Q))
    {
        if (m_playerInterface && m_playerInterface->GetCurSkillCooldown(0) <= 0.0f)
        {
            m_navMeshAgent->Stop();

            RequestStateChange(PlayerStateType::Skill_1);

            if (m_animationStateMachine)
                m_animationStateMachine->RequestStateChange(AnimationStateType::Skill_1);

            OnSkillUsed(0, nullptr);
        }
    }

    if (INPUT->GetButtonDown(KEY_TYPE::W))
    {
        m_navMeshAgent->Stop();
        if (m_playerInterface && m_playerInterface->GetCurSkillCooldown(1) <= 0.0f)
        {
            RequestStateChange(PlayerStateType::Skill_2);

            if (m_animationStateMachine)
                m_animationStateMachine->RequestStateChange(AnimationStateType::Skill_2);

            OnSkillUsed(1, nullptr);
        }
    }

    if (INPUT->GetButtonDown(KEY_TYPE::E))
    {
        m_navMeshAgent->Stop();
        if (m_playerInterface && m_playerInterface->GetCurSkillCooldown(2) <= 0.0f)
        {
            RequestStateChange(PlayerStateType::Skill_3);

            if (m_animationStateMachine)
                m_animationStateMachine->RequestStateChange(AnimationStateType::Skill_3);

            OnSkillUsed(2, nullptr);
        }
    }
    if (INPUT->GetButtonDown(KEY_TYPE::R))
    {
        if (m_playerInterface && m_playerInterface->GetCurSkillCooldown(3) <= 0.0f)
        {
            // 스킬 메타 데이터 조회
            const auto& rSkillMeta = SkillConfig::GetSkillMetaData(m_characterIndex, 3);

            bool canCast = false;
            shared_ptr<GameObject> target = nullptr;

            switch (rSkillMeta.targetType)
            {
            case SkillTargetType::Single:
                // 단일 타겟 필요 (니키)
                if (CheckTargetForSkill(KEY_TYPE::R))
                {
                    target = GetPickedTargetAtMouse();
                    if (target)
                    {
                        cout << "R 스킬: 단일 타겟 설정 완료" << endl;
                        m_navMeshAgent->Stop();
                        canCast = true;
                    }
                }
                else
                {
                    cout << "R 스킬: 유효한 타겟이 없습니다." << endl;
                }
                break;

            case SkillTargetType::None:
                // 타겟 불필요 (비앙카)
                cout << "R 스킬: 타겟 없이 발동" << endl;
                m_navMeshAgent->Stop();
                canCast = true;
                break;

                // 다른 타겟 타입들...
            }

            if (canCast)
            {
                RequestStateChange(PlayerStateType::Skill_4);

                if (m_animationStateMachine)
                    m_animationStateMachine->RequestStateChange(AnimationStateType::Skill_4);

                OnSkillUsed(3, target);
            }
        }
    }
}

void PlayerStateMachine::CheckQSkillCompletion()
{
     // Skill_1 상태일 때만 완료 체크
    if (IsInState(PlayerStateType::Skill_1))
    {
        // 이미 완료 체크를 했으면 건너뛰기
        if (m_qSkillCompletionChecked)
            return;

        bool qSkillCompleted = false;
        OnQSkillCompleted(qSkillCompleted);

        if (qSkillCompleted)
        {
            cout << "Q스킬 완료 감지 - Wait 상태로 전환" << endl;

            m_qSkillCompletionChecked = true;  // 플래그 설정

            RequestStateChange(PlayerStateType::Wait);

            if (m_animationStateMachine)
            {
                m_animationStateMachine->RequestStateChange(AnimationStateType::Wait);
            }
        }
    }
    else
    {
        // Skill_1 상태가 아니면 플래그 리셋
        m_qSkillCompletionChecked = false;
    }
}

void PlayerStateMachine::CheckWSkillCompletion()
{
    // Skill_1 상태일 때만 완료 체크
    if (IsInState(PlayerStateType::Skill_2))
    {
        // 이미 완료 체크를 했으면 건너뛰기
        if (m_wSkillCompletionChecked)
            return;

        bool wSkillCompleted = false;
        OnWSkillCompleted(wSkillCompleted);

        if (wSkillCompleted)
        {
            cout << "W스킬 완료 감지 - Wait 상태로 전환" << endl;

            m_wSkillCompletionChecked = true;  // 플래그 설정

            RequestStateChange(PlayerStateType::Wait);

            if (m_animationStateMachine)
            {
                m_animationStateMachine->RequestStateChange(AnimationStateType::Wait);
            }
        }
    }
    else
    {
        // Skill_2 상태가 아니면 플래그 리셋
        m_wSkillCompletionChecked = false;
    }
}

void PlayerStateMachine::CheckESkillCompletion()
{
     // Skill_3 상태일 때만 완료 체크
    if (IsInState(PlayerStateType::Skill_3))
    {
        // 이미 완료 체크를 했으면 건너뛰기
        if (m_eSkillCompletionChecked)
            return;

        bool eSkillCompleted = false;
        OnESkillCompleted(eSkillCompleted);

        if (eSkillCompleted)
        {
            cout << "E스킬 완료 감지 - Wait 상태로 전환" << endl;

            m_eSkillCompletionChecked = true;  // 플래그 설정

            RequestStateChange(PlayerStateType::Wait);

            if (m_animationStateMachine)
            {
                m_animationStateMachine->RequestStateChange(AnimationStateType::Wait);
            }
        }
    }
    else
    {
        // Skill_3 상태가 아니면 플래그 리셋
        m_eSkillCompletionChecked = false;
    }
}

void PlayerStateMachine::CheckRSkillCompletion()
{
    // Skill_4 상태일 때만 완료 체크
    if (IsInState(PlayerStateType::Skill_4))
    {
        // 이미 완료 체크를 했으면 건너뛰기
        if (m_rSkillCompletionChecked)
            return;

        bool rSkillCompleted = false;
        OnRSkillCompleted(rSkillCompleted);

        if (rSkillCompleted)
        {
            cout << "R스킬 완료 감지 - Wait 상태로 전환" << endl;

            m_rSkillCompletionChecked = true;  // 플래그 설정

            RequestStateChange(PlayerStateType::Wait);

            if (m_animationStateMachine)
            {
                m_animationStateMachine->RequestStateChange(AnimationStateType::Wait);
            }
        }
    }
    else
    {
        // Skill_4 상태가 아니면 플래그 리셋
        m_rSkillCompletionChecked = false;
    }
}

void PlayerStateMachine::HandleCraftInput()
{
    if (INPUT->GetButtonDown(KEY_TYPE::Z))
    {
        //이동은 Wait나 Run 상태일때만
        if (!(IsInState(PlayerStateType::Run) == true || IsInState(PlayerStateType::Wait) == true))
        {
            return;
        }

        bool craftSuccess = false;
        OnTryCraft(craftSuccess);  // Client에게 제작 가능 여부 확인 요청

        if (craftSuccess)
        {
            cout << "제작 시작 - Craft 상태로 전환" << endl;

            // NavMeshAgent 정지
            if (m_navMeshAgent)
            {
                m_navMeshAgent->Stop();
            }

            // Player 상태를 Craft로 변경
            RequestStateChange(PlayerStateType::Craft);

            // 애니메이션도 Craft로 변경
            if (m_animationStateMachine)
            {
                m_animationStateMachine->RequestStateChange(AnimationStateType::Craft);
            }
        }
        else
        {
            cout << "제작할 수 있는 재료가 없습니다." << endl;
        }
    }
}

// 새로운 메서드 추가
void PlayerStateMachine::CheckCraftCompletion()
{
    // Craft 상태일 때만 완료 체크
    if (IsInState(PlayerStateType::Craft))
    {
        bool craftCompleted = false;
        OnCraftCompleted(craftCompleted);  // Client에게 제작 완료 여부 확인

        if (craftCompleted)
        {
            cout << "제작 완료 감지 - Wait 상태로 전환" << endl;

            // Player 상태를 Wait로 변경
            RequestStateChange(PlayerStateType::Wait);

            // 애니메이션도 Wait로 변경
            if (m_animationStateMachine)
            {
                m_animationStateMachine->RequestStateChange(AnimationStateType::Wait);
            }
        }
    }
}


void PlayerStateMachine::HandleAttackInput()
{
    m_baseAttackDelayDuration += DT;
    if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
    {
        // Wait나 Run 상태에서만 기본공격 가능
        if (!(IsInState(PlayerStateType::Wait) || IsInState(PlayerStateType::Run)))
        {
            return;
        }

        // 기본공격 쿨타임 체크
        
        if (m_baseAttackDelayDuration < m_baseAttackDelay)
        {
            cout << "기본공격 쿨타임 중: " << (m_baseAttackDelay - m_baseAttackDelayDuration) << "초 남음" << endl;
            return;
        }

        // 공격 대상 피킹
        auto attackTarget = GetPickedTargetAtMouse();

        if (attackTarget && IsValidAttackTarget(attackTarget))
        {
            cout << "기본공격 대상 설정: " << attackTarget->GetName().c_str() << endl;

            // 쿨타임 리셋
            m_baseAttackDelayDuration = 0.f;

            // 기본공격 상태로 전환
            RequestStateChange(PlayerStateType::BaseAttack);

            if (m_animationStateMachine)
                m_animationStateMachine->RequestStateChange(AnimationStateType::BaseAttack);

            // 타겟 설정
            GetState(PlayerStateType::BaseAttack)->SetTarget(attackTarget);
        }
        else
        {
            // 타겟이 없으면 일반 이동 처리 (기존 코드)
            HandleMovementInput();
        }
    }
}

// 유효한 공격 대상인지 확인하는 함수 추가
bool PlayerStateMachine::IsValidAttackTarget(shared_ptr<GameObject> target)
{
    if (!target) return false;

    // 몬스터인지 확인 (Collider 존재 여부로 판단)
    if (!target->GetCollider()) return false;

    // 몬스터 타입 확인
    return target->GetType() == OBJECTTYPE::MONSTER;
}

void PlayerStateMachine::HandleRightClickInput()
{
    m_baseAttackDelayDuration += DT;

    if (!INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
        return;

    // Wait나 Run 상태에서만 처리
    if (!(IsInState(PlayerStateType::Run) || IsInState(PlayerStateType::Wait) || IsInState(PlayerStateType::BaseAttack)))
        return;

    // 1. 먼저 공격 대상이 있는지 확인
    auto attackTarget = GetPickedTargetAtMouse();

    if (attackTarget && IsValidAttackTarget(attackTarget))
    {
        // 공격 쿨타임 체크
        if (m_baseAttackDelayDuration >= m_baseAttackDelay)
        {
            cout << "기본공격 실행" << endl;

            m_baseAttackDelayDuration = 0.f;
            RequestStateChange(PlayerStateType::BaseAttack);

           /* if (m_animationStateMachine)
                m_animationStateMachine->RequestStateChange(AnimationStateType::BaseAttack);*/

            GetState(PlayerStateType::BaseAttack)->SetTarget(attackTarget);
        }
        else
        {
            cout << "기본공격 쿨타임 중" << endl;
        }
    }
    else
    {
        // 공격 대상이 없으면 이동 처리
        HandleMovementInput();
    }
}
//
//void PlayerStateMachine::ProcessInput()
//{
//
//    if ((GetCurrentState() == PlayerStateType::Skill_1) && ((1 << 3) & m_isMovableOnSkill))
//    {
//        if ((1 << 3) & m_chargingInfo)
//        {
//            if (!m_currentState->IsMovable())
//            {
//                // 이동 불가능한 상태 (Release 중)
//                return;
//            }
//        }
//        else
//        {
//            //cout << "Q 스킬 중 이동 금지\n";
//
//            return;
//        }
// 
//    }
//    if ((GetCurrentState() == PlayerStateType::Skill_2) && ((1 << 2) & m_isMovableOnSkill))
//    {
//        //cout << "W 스킬 중 이동 금지\n";
//        return;
//    }
//    if ((GetCurrentState() == PlayerStateType::Skill_3) && ((1 << 1) & m_isMovableOnSkill))
//    {
//        if ((1 << 1) & m_chargingInfo)
//        {
//            if (!m_currentState->IsMovable())
//            {
//                // 이동 불가능한 상태 (Release 중)
//                return;
//            }
//        }
//        else
//        {
//            //cout << "E 스킬 중 이동 금지\n";
//
//            return;
//        }
//    }
//    if ((GetCurrentState() == PlayerStateType::Skill_4) && ((1 << 0) & m_isMovableOnSkill))
//    {
//        //cout << "R 스킬 중 이동 금지\n";
//        return;
//    }
//    if ((GetCurrentState() == PlayerStateType::Craft))
//    {
//        //cout << "W 스킬 중 이동 금지\n";
//        return;
//    }
//    if (((GetCurrentState() == PlayerStateType::Counter)))
//    {
//        cout << "카운터 공격 중 이동금지\n";
//        return;
//    }
//
//
//
//    // NavMeshAgent 가져오기 - 올바른 ComponentType 사용
//    auto gameObject = GetGameObject();
//    auto navMeshAgent = gameObject->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);
//
//    if (!navMeshAgent)
//    {
//        cout << "NavMeshAgent not found!" << endl;
//        return;
//    }
//
//    m_baseAttackDelayDuration += DT;
//    // 우클릭 처리
//    if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
//    {
//        // 우클릭으로 공격 대상 피킹
//        auto attackTarget = CURSCENE->GetObjectManager()->PickObjectForAttack(GetGameObject());
//
//        //cout << "BaseAttackDelay : " << m_baseAttackDelayDuration << endl;
//        if (attackTarget && m_baseAttackDelay <= m_baseAttackDelayDuration)
//        {
//            m_baseAttackDelayDuration = 0.f;
//            m_states[PlayerStateType::BaseAttack]->SetTarget(attackTarget);
//            ChangeState(PlayerStateType::BaseAttack);
//        }
//        else if (attackTarget == nullptr)
//        {
//            m_states[PlayerStateType::BaseAttack]->SetTarget(nullptr);
//            // 마우스 위치 유효성 검사
//            POINT mousePos = INPUT->GetMousePos();
//            if (mousePos.x < 0 || mousePos.y < 0) return;
//
//            auto camera = CURSCENE->GetMainCamera();
//            if (!camera) return;
//
//            auto cameraComp = camera->GetCamera();
//            if (!cameraComp) return;
//
//            Ray ray = CreateRayFromMouse(mousePos, cameraComp);
//
//            // NavMesh 찾기 및 Ray cast
//            bool foundDestination = false;
//            for (auto& obj : CURSCENE->GetObjects())
//            {
//                auto navMesh = obj->GetFixedComponent<NavMesh>(ComponentType::NavMesh);
//                if (navMesh)
//                {
//                    Vec3 hitPoint;
//                    if (navMesh->RaycastNavMesh(ray, hitPoint))
//                    {
//                        navMeshAgent->SetDestination(hitPoint);
//                        foundDestination = true;
//
//                        // 이동 명령이 성공하면 즉시 Run 상태로 전환
//                        if (CanChangeState(PlayerStateType::Run))
//                        {
//                            ChangeState(PlayerStateType::Run);
//                            m_animationStateMachine->ChangeState(AnimationStateType::Run);
//                        }
//                        break;
//                    }
//                }
//            }
//
//            if (!foundDestination)
//            {
//                cout << "No valid destination found on NavMesh" << endl;
//            }
//        }
//    }
//
//    // NavMeshAgent 상태 지속적 모니터링 (이동 완료 감지용)
//    if (navMeshAgent->HasReachedDestination() && IsInState(PlayerStateType::Run))
//    {
//
//        ChangeState(PlayerStateType::Wait);
//        m_animationStateMachine->ChangeState(AnimationStateType::Wait);
//    }
//
//    if (INPUT->GetButtonDown(KEY_TYPE::LCTRL) || INPUT->GetButton(KEY_TYPE::LCTRL))
//    {
//        cout << "왼쪽컨트롤 키 눌림\n";
//        return;
//    }
//
//    // 스킬 입력 시 이동 중지
//    if (INPUT->GetButtonDown(KEY_TYPE::Q))
//    {
//        
//        float curCoolDown = m_playerInterface->GetCurSkillCooldown(0);
//        float maxCoolDown = m_playerInterface->GetMaxSkillCooldown(0);
//
//        cout << "최대 쿨타임 : " << maxCoolDown << ", 현재 쿨타임 : " << curCoolDown << endl;
//        
//        if (curCoolDown > 0.f) return;
//
//        navMeshAgent->Stop(); // 이동 중지
//        m_animationStateMachine->ChangeState(AnimationStateType::Skill_1);
//        ChangeState(PlayerStateType::Skill_1);
//
//        OnSkillUsed(0,nullptr);  // 0: Q 스킬 인덱스
//    }
//    if (INPUT->GetButtonDown(KEY_TYPE::W))
//    {
//        float curCoolDown = m_playerInterface->GetCurSkillCooldown(1);
//        float maxCoolDown = m_playerInterface->GetMaxSkillCooldown(1);
//
//        cout << "최대 쿨타임 : " << maxCoolDown << ", 현재 쿨타임 : " << curCoolDown << endl;
//
//        if (curCoolDown > 0.f) return;
//
//        navMeshAgent->Stop();
//        m_animationStateMachine->ChangeState(AnimationStateType::Skill_2);
//        ChangeState(PlayerStateType::Skill_2);
//
//        OnSkillUsed(1, nullptr);  // 1: W 스킬 인덱스
//    }
//    if (INPUT->GetButtonDown(KEY_TYPE::E))
//    {
//        float curCoolDown = m_playerInterface->GetCurSkillCooldown(2);
//        float maxCoolDown = m_playerInterface->GetMaxSkillCooldown(2);
//
//        cout << "최대 쿨타임 : " << maxCoolDown << ", 현재 쿨타임 : " << curCoolDown << endl;
//
//        if (curCoolDown > 0.f) return;
//
//        navMeshAgent->Stop();
//        m_animationStateMachine->ChangeState(AnimationStateType::Skill_3);
//        ChangeState(PlayerStateType::Skill_3);
//
//        OnSkillUsed(2, nullptr);  // 2: E 스킬 인덱스
//    }
//    if (INPUT->GetButtonDown(KEY_TYPE::R))
//    {
//        float curCoolDown = m_playerInterface->GetCurSkillCooldown(3);
//        float maxCoolDown = m_playerInterface->GetMaxSkillCooldown(3);
//
//        cout << "최대 쿨타임 : " << maxCoolDown << ", 현재 쿨타임 : " << curCoolDown << endl;
//
//        if (curCoolDown > 0.f) return;
//
//        if (m_isNeedTarget & (1 << 0))
//        {
//            // R 스킬은 타겟이 필요함
//            if (CheckTargetForSkill(KEY_TYPE::R))
//            {
//                auto target = GetPickedTargetAtMouse();
//                navMeshAgent->Stop();
//                OnSkillUsed(3, target);  // R은 타겟 정보 전달
//                m_animationStateMachine->ChangeState(AnimationStateType::Skill_4);
//                ChangeState(PlayerStateType::Skill_4);
//
//            }
//            else
//            {
//                cout << "R 스킬: 유효한 타겟이 없습니다." << endl;
//            }
//        }
//        else
//        {
//            navMeshAgent->Stop();
//            m_animationStateMachine->ChangeState(AnimationStateType::Skill_4);
//            ChangeState(PlayerStateType::Skill_4);
//
//            OnSkillUsed(3, nullptr);  // 3: R 스킬 인덱스 
//        }
//    }
//    // PlayerStateMachine.cpp - Z키 처리
//    if (INPUT->GetButtonDown(KEY_TYPE::Z))
//    {
//        bool craftSuccess = false;
//        OnTryCraftFirst(craftSuccess);  // Client에게 "첫 번째 조합 시도해줘" 요청
//       
//        if (!craftSuccess) return;
//
//        // 애니메이션 전환
//        auto navMeshAgent = GetGameObject()
//            ->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);
//        if (navMeshAgent)
//            navMeshAgent->Stop();
//
//        m_animationStateMachine->ChangeState(AnimationStateType::Craft);
//        ChangeState(PlayerStateType::Craft);     
//    }  
//}


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
