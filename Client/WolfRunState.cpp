#include "pch.h"
#include "WolfRunState.h"

WolfRunState::WolfRunState()
    : AnimationState(AnimationStateType::Move)
{
}

void WolfRunState::Enter(shared_ptr<ModelAnimator> _animator)
{
}

void WolfRunState::Update(shared_ptr<ModelAnimator> _animator)
{
}

void WolfRunState::Exit(shared_ptr<ModelAnimator> _animator)
{
}

bool WolfRunState::CanTransitionTo(AnimationStateType _nextState)
{
    return false;
}
