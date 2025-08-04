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

PlayerStateMachine::PlayerStateMachine(shared_ptr<AnimationStateMachine> animationStateMachine, int chargingInfo, int isMovableOnSkill)
    : Component(ComponentType::PlayerStateMachine) 
    , m_currentState(nullptr)
    , m_animationStateMachine(animationStateMachine)
    , m_chargingInfo(chargingInfo)
    , m_isMovableOnSkill(isMovableOnSkill)
{
    
}

PlayerStateMachine::~PlayerStateMachine()
{
    
}

void PlayerStateMachine::Init()
{
    shared_ptr<ModelAnimator> modelAnimator = GetGameObject()->GetModelAnimator();
    m_states[PlayerStateType::Wait] = make_shared<PlayerWaitState>();
    m_states[PlayerStateType::Run] = make_shared<PlayerRunState>();
    m_states[PlayerStateType::Skill_1] = make_shared<PlayerQState>(modelAnimator, m_chargingInfo & 8);
    m_states[PlayerStateType::Skill_2] = make_shared<PlayerWState>(modelAnimator);
    m_states[PlayerStateType::Skill_3] = make_shared<PlayerEState>(modelAnimator, m_chargingInfo & 2);
    m_states[PlayerStateType::Skill_4] = make_shared<PlayerRState>(modelAnimator);

    // 초기화 작업
    m_currentState = m_states[PlayerStateType::Wait];
}

void PlayerStateMachine::Start()
{
    Init();
    //ChangeState("Wait");
}

void PlayerStateMachine::Update()
{
    ProcessInput();
   
    if (m_currentState)
    {
        m_currentState->Update();

        HandleSpecialStateTransitions();
    }
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
    if (!CanChangeState(newState))
        return;

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
            bool IsCharging = static_pointer_cast<PlayerQState>(m_currentState)->m_isCharging;
            if (!IsCharging) 
            {
                //cout << "Q 스킬 중 이동 금지\n";
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
            bool IsCharging = static_pointer_cast<PlayerEState>(m_currentState)->m_isCharging;
            if (!IsCharging)
            {
                //cout << "E 스킬 중 이동 금지\n";
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




    // NavMeshAgent 가져오기 - 올바른 ComponentType 사용
    auto gameObject = GetGameObject();
    auto navMeshAgent = gameObject->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);

    if (!navMeshAgent)
    {
        cout << "NavMeshAgent not found!" << endl;
        return;
    }

    // 우클릭 처리
    if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
    {
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

    // NavMeshAgent 상태 지속적 모니터링 (이동 완료 감지용)
    if (navMeshAgent->HasReachedDestination() && IsInState(PlayerStateType::Run))
    {
        ChangeState(PlayerStateType::Wait);
        m_animationStateMachine->ChangeState(AnimationStateType::Wait);
    }

   

    // 스킬 입력 시 이동 중지
    if (INPUT->GetButtonDown(KEY_TYPE::Q))
    {
        navMeshAgent->Stop(); // 이동 중지
        m_animationStateMachine->ChangeState(AnimationStateType::Skill_1);
        ChangeState(PlayerStateType::Skill_1);

        OnSkillUsed(0);  // 0: Q 스킬 인덱스
    }
    if (INPUT->GetButtonDown(KEY_TYPE::W))
    {
        navMeshAgent->Stop();
        m_animationStateMachine->ChangeState(AnimationStateType::Skill_2);
        ChangeState(PlayerStateType::Skill_2);

        OnSkillUsed(1);  // 1: W 스킬 인덱스
    }
    if (INPUT->GetButtonDown(KEY_TYPE::E))
    {
        navMeshAgent->Stop();
        m_animationStateMachine->ChangeState(AnimationStateType::Skill_3);
        ChangeState(PlayerStateType::Skill_3);

        OnSkillUsed(2);  // 2: E 스킬 인덱스
    }
    if (INPUT->GetButtonDown(KEY_TYPE::R))
    {
        navMeshAgent->Stop();
        m_animationStateMachine->ChangeState(AnimationStateType::Skill_4);
        ChangeState(PlayerStateType::Skill_4);

        OnSkillUsed(3);  // 3: R 스킬 인덱스 
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

    //// 디버깅 출력
    //cout << "=== Ray Generation Debug ===" << endl;
    //cout << "Mouse Position: (" << mousePos.x << ", " << mousePos.y << ")" << endl;
    //cout << "Viewport: " << viewport.GetWidth() << "x" << viewport.GetHeight() << endl;
    //cout << "Near Point: (" << nearPoint.x << ", " << nearPoint.y << ", " << nearPoint.z << ")" << endl;
    //cout << "Far Point: (" << farPoint.x << ", " << farPoint.y << ", " << farPoint.z << ")" << endl;
    //cout << "Ray Direction: (" << rayDirection.x << ", " << rayDirection.y << ", " << rayDirection.z << ")" << endl;

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
            auto gameObject = GetGameObject();
            auto navMeshAgent = gameObject->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);
            navMeshAgent->Stop();
            ChangeState(PlayerStateType::Wait);
        }
    }
    // W 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == PlayerStateType::Skill_2)
    {
        if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            ChangeState(PlayerStateType::Wait);
        }
    }
    // E 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == PlayerStateType::Skill_3)
    {
        if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            auto gameObject = GetGameObject();
            auto navMeshAgent = gameObject->GetFixedComponent<NavMeshAgent>(ComponentType::NavMeshAgent);
            navMeshAgent->Stop();
            ChangeState(PlayerStateType::Wait);
        }
    }
    // R 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == PlayerStateType::Skill_4)
    {
        if (m_currentState->CanTransitionTo(PlayerStateType::Wait))
        {
            ChangeState(PlayerStateType::Wait);
        }
    }

    // 다른 스킬들도 동일하게 처리 가능
    // if (GetCurrentState() == AnimationStateType::Skill_1) { ... }
}