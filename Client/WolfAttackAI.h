#pragma once
#include "AI.h"
class WolfAttackAI :
    public AI
{
public:
	WolfAttackAI();
	virtual ~WolfAttackAI();

public:
	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
};

