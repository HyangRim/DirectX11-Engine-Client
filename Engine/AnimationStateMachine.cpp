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

AnimationStateMachine::AnimationStateMachine()
    : Super(ComponentType::AnimationStateMachine)
{
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
    m_currentState = m_states[AnimationStateType::Wait];
    if (m_currentState)
    {
        m_currentState->Enter(m_animator);
    }
}

void AnimationStateMachine::Update()
{
    Super::Update();

   // ProcessInput();

    if (m_currentState)
    {
        m_currentState->Update(m_animator);     

        // 특수 상태 처리 추가
        HandleSpecialStateTransitions();
    }
}

//void AnimationStateMachine::ProcessInput()
//{
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
//    // 우클릭 처리
//    if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
//    {
//        // 마우스 위치 유효성 검사
//        POINT mousePos = INPUT->GetMousePos();
//        if (mousePos.x < 0 || mousePos.y < 0) return;
//
//        auto camera = CURSCENE->GetMainCamera();
//        if (!camera) return;
//
//        auto cameraComp = camera->GetCamera();
//        if (!cameraComp) return;
//
//        Ray ray = CreateRayFromMouse(mousePos, cameraComp);
//
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
//                    navMeshAgent->SetDestination(hitPoint);
//                    foundDestination = true;
//
//                    // 이동 명령이 성공하면 즉시 Run 상태로 전환
//                    if (CanChangeState(AnimationStateType::Run))
//                    {
//                        ChangeState(AnimationStateType::Run);
//                    }
//                    break;
//                }
//            }
//        }
//
//        if (!foundDestination)
//        {
//            cout << "No valid destination found on NavMesh" << endl;
//        }
//    }
//
//    // NavMeshAgent 상태 지속적 모니터링 (이동 완료 감지용)
//    if (navMeshAgent->HasReachedDestination() && IsInState(AnimationStateType::Run))
//    {
//        ChangeState(AnimationStateType::Wait);
//    }
//
//    // 스킬 입력 시 이동 중지
//    if (INPUT->GetButtonDown(KEY_TYPE::B))
//    {
//        navMeshAgent->Stop(); // 이동 중지
//        ChangeState(AnimationStateType::Skill_1);
//    }
//    if (INPUT->GetButtonDown(KEY_TYPE::W))
//    {
//        navMeshAgent->Stop();
//        ChangeState(AnimationStateType::Skill_2);
//    }
//    if (INPUT->GetButtonDown(KEY_TYPE::E))
//    {
//        navMeshAgent->Stop();
//        ChangeState(AnimationStateType::Skill_3);
//    }
//    if (INPUT->GetButtonDown(KEY_TYPE::R))
//    {
//        navMeshAgent->Stop();
//        ChangeState(AnimationStateType::Skill_4);
//    }
//}

void AnimationStateMachine::ChangeState(AnimationStateType newState)
{
    if (!CanChangeState(newState))
        return;

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
}

bool AnimationStateMachine::CanChangeState(AnimationStateType newState)
{
    if (!m_currentState)
        return true;

    return m_currentState->CanTransitionTo(newState);
}

AnimationStateType AnimationStateMachine::GetCurrentState() const
{
    return m_currentState ? m_currentState->GetType() : AnimationStateType::Wait;
}

shared_ptr<AnimationState> AnimationStateMachine::GetCurrentState2(AnimationStateType type)
{
    return m_states[type];
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
    // W 스킬 완료 후 Wait 상태로 전환
    else if (GetCurrentState() == AnimationStateType::Skill_4)
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

//
//Ray AnimationStateMachine::CreateRayFromMouse(POINT mousePos, shared_ptr<Camera> camera)
//{
//    Viewport viewport = GRAPHICS->GetViewport();
//    Matrix worldMatrix = Matrix::Identity;
//    Matrix viewMatrix = camera->GetViewMatrix();
//    Matrix projMatrix = camera->GetProjectionMatrix();
//
//    // 화면 좌표를 NDC로 변환
//    float x = (2.0f * mousePos.x) / viewport.GetWidth() - 1.0f;
//    float y = 1.0f - (2.0f * mousePos.y) / viewport.GetHeight();
//
//    // Near와 Far 평면의 월드 좌표 계산
//    Vec3 nearPoint = viewport.UnProject(Vec3(mousePos.x, mousePos.y, 0.0f), worldMatrix, viewMatrix, projMatrix);
//    Vec3 farPoint = viewport.UnProject(Vec3(mousePos.x, mousePos.y, 1.0f), worldMatrix, viewMatrix, projMatrix);
//
//    Vec3 rayDirection = farPoint - nearPoint;
//    rayDirection.Normalize();
//
//    // 디버깅 출력
//    cout << "=== Ray Generation Debug ===" << endl;
//    cout << "Mouse Position: (" << mousePos.x << ", " << mousePos.y << ")" << endl;
//    cout << "Viewport: " << viewport.GetWidth() << "x" << viewport.GetHeight() << endl;
//    cout << "Near Point: (" << nearPoint.x << ", " << nearPoint.y << ", " << nearPoint.z << ")" << endl;
//    cout << "Far Point: (" << farPoint.x << ", " << farPoint.y << ", " << farPoint.z << ")" << endl;
//    cout << "Ray Direction: (" << rayDirection.x << ", " << rayDirection.y << ", " << rayDirection.z << ")" << endl;
//
//    // Ray 방향이 아래쪽을 향하는지 확인
//    if (rayDirection.y > 0) {
//        cout << "WARNING: Ray pointing upward!" << endl;
//    }
//
//    return Ray(nearPoint, rayDirection);
//}
