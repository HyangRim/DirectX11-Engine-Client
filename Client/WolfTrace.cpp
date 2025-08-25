#include "pch.h"
#include "WolfTrace.h"
#include "GameObject.h"
#include "NavMeshAgent.h"

#include "MonsterStateMachine.h"
#include "NickyBaseAttack.h"



WolfTrace::WolfTrace() {}

void WolfTrace::Start()
{
    m_navAgent = m_owner->GetNavMeshAgent();
}

void WolfTrace::Update()
{
    if (!m_target || !m_owner || !m_navAgent)
        return;

    Vec3 otherObjPos = m_target->GetTransform()->GetPosition();
    Vec3 wolfPos = m_owner->GetTransform()->GetPosition();

    CalcDir(otherObjPos, wolfPos);

    m_updateTimer += DT;
    if (m_updateTimer >= m_pathUpdateInterval) 
    {
        m_navAgent->SetDestination(m_target->GetTransform()->GetPosition());
        m_navAgent->SetSpeed(m_traceSpeed);
        m_updateTimer = 0.0f;
    }

    // 공격 사거리 진입시 상태머신에게 알림, 혹은 직접 상태 전환을 유도할 수도 있음
    float distance = Vec3::Distance(otherObjPos, wolfPos);


    if (distance < m_attackRange)
    {
        // ex) 상태머신의 상태 바꾸기
        // 혹은 콜백/Delegate로 공격동작 시작 호출
        auto msm = m_owner->GetMonsterStateMachine();
        if (msm && msm->CanChangeState(MonsterStateType::Attack))
        {
            cout << "공격 거리 이내 - Attack State 상태로 전환" << endl;
            //msm->ChangeState(MonsterStateType::Attack);
            auto animSM = m_owner->GetAnimationStateMachine();
            if (animSM) {
                //animSM->ChangeState(AnimationStateType::BaseAttack);
            }
        }
    }
}

void WolfTrace::StartTrace()
{
    SetActive(true);
    m_updateTimer = 0.0f;
}

void WolfTrace::StopTrace()
{
    SetActive(false);
    if (m_navAgent)
        m_navAgent->Stop();
}

void WolfTrace::CalcDir(Vec3 otherPos, Vec3 wolfPos)
{ 
    Vec3 dir = otherPos - wolfPos;
    dir.Normalize();

    // 회전 계산 및 적용
    float targetYaw = atan2(dir.x, dir.z) + 3.141592f;
    Vec3 currentRotation = m_owner->GetTransform()->GetLocalRotation();
    Vec3 newRotation = Vec3(currentRotation.x, (targetYaw * 180.0f / 3.14159f), currentRotation.z);

    m_owner->GetTransform()->SetLocalRotation(newRotation);
}

bool WolfTrace::IsInAttackRange()
{
    if (!m_target || !m_owner) return false;

    float distance = Vec3::Distance(
        m_owner->GetTransform()->GetPosition(),
        m_target->GetTransform()->GetPosition()
    );
    return distance < m_attackRange;
}