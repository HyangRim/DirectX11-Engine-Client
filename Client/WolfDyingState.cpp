#include "pch.h"
#include "WolfDyingState.h"

WolfDyingState::WolfDyingState()
	: AnimationState(AnimationStateType::Dead)
{
}

void WolfDyingState::Enter(shared_ptr<ModelAnimator> _animator)
{
}

void WolfDyingState::Update(shared_ptr<ModelAnimator> _animator)
{
}

void WolfDyingState::Exit(shared_ptr<ModelAnimator> _animator)
{
}

bool WolfDyingState::CanTransitionTo(AnimationStateType _nextState)
{
	return false;
}
