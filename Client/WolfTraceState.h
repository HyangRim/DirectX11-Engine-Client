#pragma once
#include "MonsterState.h"
class WolfTraceState :
    public MonsterState
{
    using Super = MonsterState;
public:
    WolfTraceState();
    virtual ~WolfTraceState() = default;

    virtual void Enter();
    virtual void Update();
    virtual void Exit();
    virtual bool CanTransitionTo(MonsterStateType newState);

private:
    bool m_isAnimationStarted;
    float m_animTime;
};

