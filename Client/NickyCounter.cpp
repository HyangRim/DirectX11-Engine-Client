#include "pch.h"
#include "NickyCounter.h"

#include "PlayerStateMachine.h"

#include "Player.h"
#include "Monster.h"

NickyCounter::NickyCounter()
{

}

void NickyCounter::Start()
{
	m_animStateMachine = m_owner->GetAnimationStateMachine();
}

void NickyCounter::Update()
{
	m_updateTimer += DT;

	if (m_updateTimer >= m_attackDuration)
	{
		/*m_owner->GetNavMeshAgent()->Stop();
		m_owner->GetPlayerStateMachine()->ChangeState(PlayerStateType::Wait);
		m_owner->GetAnimationStateMachine()->ChangeState(AnimationStateType::Wait);*/

		return;
	}
}

void NickyCounter::StartCounter()
{
	SetActive(true);
	m_updateTimer = 0.f;
}

void NickyCounter::StopCounter()
{
	SetActive(false);

	
	m_updateTimer = 0.f;
}

void NickyCounter::CalcDir(Vec3 otherPos, Vec3 wolfPos)
{
	Vec3 dir = otherPos - wolfPos;
	dir.Normalize();

	// 회전 계산 및 적용
	float targetYaw = atan2(dir.x, dir.z) + 3.141592f;
	Vec3 currentRotation = m_owner->GetTransform()->GetLocalRotation();
	Vec3 newRotation = Vec3(currentRotation.x, (targetYaw * 180.0f / 3.14159f), currentRotation.z);

	m_owner->GetTransform()->SetLocalRotation(newRotation);
}

bool NickyCounter::IsInAttackRange()
{
	return true;
}