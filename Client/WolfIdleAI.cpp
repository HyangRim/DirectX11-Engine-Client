#include "pch.h"
#include "WolfIdleAI.h"
#include "Monster.h"

WolfIdleAI::WolfIdleAI(shared_ptr<Monster> _Owner)
	: Super(_Owner)
{
}

WolfIdleAI::~WolfIdleAI()
{
}

void WolfIdleAI::Enter()
{
	m_Owner->GetAnimationStateMachine()->ChangeState(AnimationStateType::Wait);
}

void WolfIdleAI::Update()
{
}

void WolfIdleAI::Exit()
{
}
