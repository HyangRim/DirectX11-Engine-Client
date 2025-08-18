#include "pch.h"
#include "WolfTraceState.h"
#include "GameObject.h"

#include "MonsterStateMachine.h"
#include "AnimationStateMachine.h"
#include "NavMeshAgent.h"

#include "WolfTrace.h"

WolfTraceState::WolfTraceState(shared_ptr<GameObject> wolf)
    :Super(MonsterStateType::Wait)
    ,m_wolf(wolf)
{

}

void WolfTraceState::Enter()
{
    m_animTime = 0.f;
    m_isAnimationStarted = true;

    auto attackScript = m_wolf->GetComponent<WolfTrace>();
    if (attackScript) {
        attackScript->SetTarget(m_otherObj);
        attackScript->StartTrace();
    }

    cout << "늑대 Trace State 진입\n";
}

void WolfTraceState::Update()
{
    //Vec3 otherObjPos = m_otherObj->GetTransform()->GetPosition();
   
    //Vec3 wolfPos = m_wolf->GetTransform()->GetPosition();

    //Vec3 dir = otherObjPos - wolfPos;
    //dir.Normalize();

    //m_startPos = wolfPos;
    //m_targetPos = wolfPos + dir * DT * m_speed;
 
    //float distance = Vec3::Distance(m_startPos, otherObjPos);
    //
    //if (distance < 3.0f)
    //{
    //    auto msm = m_wolf->GetMonsterStateMachine();
    //    if (msm && msm->CanChangeState(MonsterStateType::Attack))
    //    {
    //        cout << "공격 거리 이내 - Attack State 상태로 전환" << endl;
    //        msm->ChangeState(MonsterStateType::Attack);
    //        auto animSM = m_wolf->GetAnimationStateMachine();
    //        if (animSM) {
    //            animSM->ChangeState(AnimationStateType::BaseAttack);
    //        }
    //    }
    //    return; // 공격 상태로 전환 후 이동 중지
    //}
 
    //// 회전 계산 및 적용
    //float targetYaw = atan2(dir.x, dir.z) + 3.141592f;
    //Vec3 currentRotation = m_wolf->GetTransform()->GetLocalRotation();
    //Vec3 newRotation = Vec3(currentRotation.x, (targetYaw * 180.0f / 3.14159f), currentRotation.z);

    //m_wolf->GetTransform()->SetLocalRotation(newRotation);
    //m_wolf->GetNavMeshAgent()->SetDestination(m_targetPos);

    //auto traceScript = m_wolf->GetComponent<WolfTrace>();
    //if (traceScript)
    //{
    //    // 스크립트에서 공격 거리 체크를 위임
    //    if (traceScript->IsInAttackRange())
    //    {
    //        auto msm = m_wolf->GetMonsterStateMachine();
    //        if (msm && msm->CanChangeState(MonsterStateType::Attack))
    //        {
    //            msm->ChangeState(MonsterStateType::Attack);
    //            auto animSM = m_wolf->GetAnimationStateMachine();
    //            if (animSM) {
    //                animSM->ChangeState(AnimationStateType::BaseAttack);
    //            }
    //        }
    //    }
    //}
}

void WolfTraceState::Exit()
{
    m_animTime = 0.f;
    m_isAnimationStarted = false;

    auto attackScript = m_wolf->GetComponent<WolfTrace>();
    if (attackScript) {
        attackScript->StopTrace();
    }

    cout << "늑대 Trace State 종료\n";
}

bool WolfTraceState::CanTransitionTo(MonsterStateType newState)
{
    switch (newState)
    {
    case MonsterStateType::Attack:
    case MonsterStateType::Death:
    case MonsterStateType::Wait:
        return true;
    default:
        return false;
    }
}