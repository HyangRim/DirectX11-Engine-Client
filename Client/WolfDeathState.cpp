#include "pch.h"
#include "WolfDeathState.h"

WolfDeathState::WolfDeathState()
    : AnimationState(AnimationStateType::Dead)
{
}

void WolfDeathState::Enter(shared_ptr<ModelAnimator> _animator)
{

}

void WolfDeathState::Update(shared_ptr<ModelAnimator> _animator)
{

}

void WolfDeathState::Exit(shared_ptr<ModelAnimator> _animator)
{

}

bool WolfDeathState::CanTransitionTo(AnimationStateType _nextState)
{
    return false;
}
