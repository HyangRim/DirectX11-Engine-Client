#pragma once
#include "AI.h"
class WolfAppearAI :
    public AI
{
public:
    WolfAppearAI();
    virtual ~WolfAppearAI();

public:
    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;
};

