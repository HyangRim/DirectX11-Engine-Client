#include "pch.h"
#include "WolfAttackState.h"

#include "GameObject.h"

#include "MonsterStateMachine.h"
#include "Player.h"

WolfAttackState::WolfAttackState(shared_ptr<GameObject> wolf)
	:Super(MonsterStateType::Attack)
	, m_wolf(wolf)
{

}

void WolfAttackState::Enter()
{
	m_animTime = 0.f;
	m_isAnimationStarted = true;
	m_isAttackComplete = false;

	cout << "´Á´ë Attack State ÁøÀÔ\n";
}

void WolfAttackState::Update()
{
	m_animTime += DT;
	if (!m_isAttackComplete && m_animTime >= (36.f / 25.f) )
	{
		Vec3 otherObjPos = m_otherObj->GetTransform()->GetPosition();
		Vec3 wolfPos = m_wolf->GetTransform()->GetPosition();

		float distance = Vec3::Distance(wolfPos, otherObjPos);

		m_animTime = 0.f;

		if (static_pointer_cast<Player>(m_wolf)->GetStatus().hp < 0 || distance >= 50.0f)
		{
			cout << "´Á´ë Attack State ¿Ï·á!" << endl;
			m_isAttackComplete = true;
		}
		else if (distance >= 10.f)
		{
			m_isAttackComplete = true;
			m_wolf->GetMonsterStateMachine()->ChangeState(MonsterStateType::Trace);
			m_wolf->GetAnimationStateMachine()->ChangeState(AnimationStateType::Trace);
			
			return;
		}
		else
			m_wolf->GetAnimationStateMachine()->ChangeState(AnimationStateType::BaseAttack);
	}
}

void WolfAttackState::Exit()
{
	m_animTime = 0.f;
	m_isAnimationStarted = false;

	cout << "´Á´ë Attack State Á¾·á\n";
}

bool WolfAttackState::CanTransitionTo(MonsterStateType newState)
{
	if (m_isAttackComplete)
	{
		switch (newState)
		{
		case MonsterStateType::Wait:
		case MonsterStateType::Trace:
		case MonsterStateType::Death:
			return true;
		default:
			return false;
		}
	}
}
