#pragma once
#include "PlayerStateMachine.h"
class BiancaRunState :
    public PlayerState
{
    using Super = PlayerState;

public:
    BiancaRunState();
    ~BiancaRunState();

    virtual void Enter();
    virtual void Update();
    virtual void Exit();
    virtual bool CanTransitionTo(PlayerStateType newState);
};

