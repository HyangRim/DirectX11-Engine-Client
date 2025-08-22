#include "pch.h"
#include "AlphaAttackState.h"

#include "GameObject.h"
#include "MonsterStateMachine.h"
#include "Player.h"

#include "AlphaBaseAttack.h"
#include "Monster.h"

AlphaAttackState::AlphaAttackState(shared_ptr<GameObject> alpha)
	:Super(MonsterStateType::Attack)
	, m_alpha(alpha)
{
}

void AlphaAttackState::Enter()
{
	m_animTime = 0.f;
	m_isAnimationStarted = true;

	// WolfBaseAttack 컴포넌트 활성화
	auto attackScript = m_alpha->GetComponent<AlphaBaseAttack>();
	if (attackScript) {
		attackScript->SetTarget(m_otherObj);
		attackScript->StartAttack();
	}

	cout << "알파 Attack State 진입\n";
}

void AlphaAttackState::Update()
{
	if (static_pointer_cast<Monster>(m_alpha)->GetMonsterStatus().hp <= 0)
	{
		m_isAttackComplete = true;
		m_alpha->GetMonsterStateMachine()->ChangeState(MonsterStateType::Death);
		m_alpha->GetAnimationStateMachine()->ChangeState(AnimationStateType::Death);

		static_pointer_cast<Monster>(m_alpha)->SetDead(true);
		return;
	}


	m_animTime += DT;

	if (!m_isAttackComplete && m_animTime >= (36.f / 25.f))
	{
		Vec3 otherObjPos = m_otherObj->GetTransform()->GetPosition();
		Vec3 wolfPos = m_alpha->GetTransform()->GetPosition();

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

void AlphaAttackState::Exit()
{
	m_animTime = 0.f;
	m_isAnimationStarted = false;

	auto attackScript = m_alpha->GetComponent<AlphaBaseAttack>();
	if (attackScript) {
		attackScript->StopAttack();
	}

	cout << "알파 Attack State 종료\n";
}

bool AlphaAttackState::CanTransitionTo(MonsterStateType newState)
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
