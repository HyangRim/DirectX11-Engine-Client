#include "pch.h"
#include "Monster.h"
#include "AI.h"
#include "Item.h"

Monster::Monster()
{
	SetType(OBJECTTYPE::MONSTER);
}

Monster::~Monster()
{
	if (m_ai != nullptr)
		m_ai.reset();
}

void Monster::Start()
{
}

void Monster::Update()
{
	if (m_ai != nullptr) {
		m_ai->Update();
	}
}

void Monster::LateUpdate()
{
}

void Monster::FixedUpdate()
{
}
