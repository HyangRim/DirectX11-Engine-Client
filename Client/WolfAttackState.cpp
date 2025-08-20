#include "pch.h"
#include "WolfAttackState.h"

#include "GameObject.h"

#include "MonsterStateMachine.h"
#include "Player.h"

#include "WolfBaseAttack.h"
#include "Monster.h"

WolfAttackState::WolfAttackState(shared_ptr<GameObject> wolf)
	:Super(MonsterStateType::Attack)
	, m_wolf(wolf)
{

}

void WolfAttackState::Enter()
{
	m_animTime = 0.f;
	m_isAnimationStarted = true;

	// WolfBaseAttack ÄÄÆ÷³ÍÆ® È°¼ºÈ­
	auto attackScript = m_wolf->GetComponent<WolfBaseAttack>();
	if (attackScript) {
		attackScript->SetTarget(m_otherObj);
		attackScript->StartAttack();
	}

	cout << "´Á´ë Attack State ÁøÀÔ\n";
}

void WolfAttackState::Update()
{
	if (static_pointer_cast<Monster>(m_wolf)->GetMonsterStatus().hp <= 0)
	{
		m_isAttackComplete = true;
		m_wolf->GetMonsterStateMachine()->ChangeState(MonsterStateType::Death);
		m_wolf->GetAnimationStateMachine()->ChangeState(AnimationStateType::Death);

		static_pointer_cast<Monster>(m_wolf)->SetDead(true);
		return;
	}


	m_animTime += DT;

	if (!m_isAttackComplete && m_animTime >= (36.f / 25.f) )
	{
		Vec3 otherObjPos = m_otherObj->GetTransform()->GetPosition();
		Vec3 wolfPos = m_wolf->GetTransform()->GetPosition();

		float distance = Vec3::Distance(wolfPos, otherObjPos);

		m_animTime = 0.f;

		if (distance >= 50.0f)
		{
			m_isAttackComplete = true;
		}
		else if (distance >= 10.f)
		{
			m_isAttackComplete = true;
			
			return;
		}	
	}
}

void WolfAttackState::Exit()
{
	m_animTime = 0.f;
	m_isAnimationStarted = false;

	auto attackScript = m_wolf->GetComponent<WolfBaseAttack>();
	if (attackScript) {
		attackScript->StopAttack();
	}

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
