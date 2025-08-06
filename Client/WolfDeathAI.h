#pragma once
#include "AI.h"
class WolfDeathAI :
    public AI
{
public:
	WolfDeathAI();
	virtual ~WolfDeathAI();

public:
	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
};

