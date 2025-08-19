#include "pch.h"
#include "NickyBaseAttackState.h"

#include "NickyBaseAttack.h"
#include "Player.h"

NickyBaseAttackState::NickyBaseAttackState(shared_ptr<ModelAnimator> modelAnimator, shared_ptr<GameObject> _player)
	:Super(PlayerStateType::BaseAttack)
	,m_player(_player)
{

}

NickyBaseAttackState::~NickyBaseAttackState()
{

}

void NickyBaseAttackState::Enter()
{
	// NickyBaseAttack 컴포넌트 활성화
	auto attackScript = m_player->GetComponent<NickyBaseAttack>();
	if (attackScript) {
		attackScript->StartBaseAttack();
		attackScript->SetTarget(m_target);
	}
}

void NickyBaseAttackState::Update()
{
	m_baseAttackTime += DT;

	if (m_player->GetComponent<NickyBaseAttack>()->GetTarget() == nullptr)
	{
		m_isBaseAttackComplete = true;
		return;
	}

	if (!m_isBaseAttackComplete && m_baseAttackTime >= (38.f / 25.f) / 2.f)
	{
		m_isBaseAttackComplete = true;	
	}
}

void NickyBaseAttackState::Exit()
{
	// NickyBaseAttack 컴포넌트 활성화
	auto attackScript = m_player->GetComponent<NickyBaseAttack>();
	if (attackScript) {
		attackScript->StopBaseAttack();
	}
}

bool NickyBaseAttackState::CanTransitionTo(PlayerStateType newState)
{
	switch (newState)
	{
	case PlayerStateType::Wait:
		return true;
	case PlayerStateType::Run:
	case PlayerStateType::Skill_1:
	case PlayerStateType::Skill_2:
	case PlayerStateType::Skill_3:
	case PlayerStateType::Skill_4:
		if (m_isBaseAttackComplete)
		{
			return true;
		}
		else return false;
	default:
		return false;
	}
}