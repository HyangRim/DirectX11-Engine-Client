#include "pch.h"
#include "WolfWaitState.h"

WolfWaitState::WolfWaitState()
    : AnimationState(AnimationStateType::Wait)
{
}

void WolfWaitState::Enter(shared_ptr<ModelAnimator> _animator)
{
}

void WolfWaitState::Update(shared_ptr<ModelAnimator> _animator)
{
}

void WolfWaitState::Exit(shared_ptr<ModelAnimator> _animator)
{
}

bool WolfWaitState::CanTransitionTo(AnimationStateType _nextState)
{
    return false;
}
