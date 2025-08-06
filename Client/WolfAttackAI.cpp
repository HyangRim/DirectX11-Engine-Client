#include "pch.h"
#include "WolfAttackAI.h"
#include "Player.h"
#include "Monster.h"

WolfAttackAI::WolfAttackAI(shared_ptr<Monster> _Owner)
	: Super(_Owner)
{
}

WolfAttackAI::~WolfAttackAI()
{
}

void WolfAttackAI::Enter()
{
	m_Owner->GetAnimationStateMachine()->ChangeState(AnimationStateType::Move);
}

void WolfAttackAI::Update()
{
	auto target = m_Owner->GetTarget();

	if (target == nullptr)
		return;

	//target과 자신의 거리 재기. 
	Vec3 ownerPosition = m_Owner->GetTransform()->GetPosition();
	Vec3 targetPosition = target->GetTransform()->GetPosition();

	
}

void WolfAttackAI::Exit()
{
}
