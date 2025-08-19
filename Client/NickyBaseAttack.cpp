#include "pch.h"
#include "NickyBaseAttack.h"

#include "PlayerStateMachine.h"

NickyBaseAttack::NickyBaseAttack()
{

}

void NickyBaseAttack::Start()
{
	m_animStateMachine = m_owner->GetAnimationStateMachine();
}

void NickyBaseAttack::Update()
{
	m_updateTimer += DT;

	Vec3 targetPos = m_target->GetTransform()->GetPosition();
	Vec3 playerPos = m_owner->GetTransform()->GetPosition();

	float distance = Vec3::Distance(targetPos, playerPos);

	//if (m_updateTimer >= m_pathUpdateInterval && distance > m_attackRange)
	//{
	//	m_owner->GetNavMeshAgent()->SetDestination(targetPos);
	//	if(m_animStateMachine->GetCurrentState() != AnimationStateType::Run)
	//		m_animStateMachine->ChangeState(AnimationStateType::Run);
	//}

	//if (!m_isAttackCompleted && distance <= m_attackRange)
	//{
	//	m_isAttackCompleted = true;
	//	m_owner->GetNavMeshAgent()->Stop();
	//	m_animStateMachine->ChangeState(AnimationStateType::BaseAttack);
	//}

	if (distance >= m_attackRange && !m_isArriveToTarget)
	{
		if (m_updateTimer > 0.1f)
		{
			m_updateTimer = 0.f;
			m_owner->GetNavMeshAgent()->SetDestination(targetPos);
			if (m_animStateMachine->GetCurrentState() != AnimationStateType::Run)
				m_animStateMachine->ChangeState(AnimationStateType::Run);
		}
	}
	else
	{
		m_owner->GetNavMeshAgent()->Stop();
		if (distance >= m_attackRange)
		{
			m_isArriveToTarget = false;
			return;
		}
		else
			m_isArriveToTarget = true;

		if (m_updateTimer >= m_attackDuration || m_owner->GetAnimationStateMachine()->GetCurrentState() != AnimationStateType::BaseAttack)
		{
			m_updateTimer = 0.f;
			m_owner->GetAnimationStateMachine()->ChangeState(AnimationStateType::BaseAttack);
		}
	}

}

void NickyBaseAttack::StartBaseAttack()
{
	SetActive(true);
	m_updateTimer = 0.f;
	//m_isAttackCompleted = false;
}

void NickyBaseAttack::StopBaseAttack()
{
	SetActive(false);
	m_updateTimer = 0.f;
	//m_isAttackCompleted = false;


}

void NickyBaseAttack::CalcDir(Vec3 otherPos, Vec3 wolfPos)
{

}

bool NickyBaseAttack::IsInAttackRange()
{
	return true;
}