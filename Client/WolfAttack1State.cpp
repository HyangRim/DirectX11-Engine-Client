#include "pch.h"
#include "WolfAttack1State.h"

WolfAttack1State::WolfAttack1State()
	: AnimationState(AnimationStateType::BaseAttack)
{
}

void WolfAttack1State::Enter(shared_ptr<ModelAnimator> _animator)
{
}

void WolfAttack1State::Update(shared_ptr<ModelAnimator> _animator)
{
}

void WolfAttack1State::Exit(shared_ptr<ModelAnimator> _animator)
{
}

bool WolfAttack1State::CanTransitionTo(AnimationStateType _nextState)
{
	return false;
}
