#pragma once
#include "AI.h"
class WolfIdleAI :
    public AI
{
public:
    WolfIdleAI();
    virtual ~WolfIdleAI();

public:
    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;
};

