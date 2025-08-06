#include "pch.h"
#include "WolfAppearState.h"

WolfAppearState::WolfAppearState()
	: AnimationState(AnimationStateType::Appear)
{
}

void WolfAppearState::Enter(shared_ptr<ModelAnimator> _animator)
{
}

void WolfAppearState::Update(shared_ptr<ModelAnimator> _animator)
{
}

void WolfAppearState::Exit(shared_ptr<ModelAnimator> _animator)
{
}

bool WolfAppearState::CanTransitionTo(AnimationStateType _nextState)
{
	return false;
}
