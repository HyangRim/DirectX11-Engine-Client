#include "pch.h"
#include "Monster.h"
#include "AI.h"
#include "Item.h"

Monster::Monster(shared_ptr<Shader> _shader)
	: m_defaultShader(_shader)
{
	SetType(OBJECTTYPE::MONSTER);
}

Monster::~Monster()
{
	if (m_curAI != nullptr)
		m_curAI.reset();
}

void Monster::Start()
{
	Super::Start();
}

void Monster::Update()
{
	if (m_curAI != nullptr) {
		m_curAI->Update();
	}
	Super::Update();
}

void Monster::LateUpdate()
{
	Super::LateUpdate();
}

void Monster::FixedUpdate()
{
	Super::FixedUpdate();
}

void Monster::ChangeState(shared_ptr<AI> _nextAI)
{
	if (_nextAI == nullptr)
		return;

	m_curAI->Exit();
	m_curAI = _nextAI;
	m_curAI->Enter();
}

void Monster::ChangeState(wstring&& _key)
{
	auto state = m_AIMap.find(_key);

	if (state == m_AIMap.end())
		wcout << "Not invalid State\n";

	m_curAI->Exit();
	m_curAI = state->second;
	m_curAI->Enter();
}

void Monster::Damaged(int _damage, shared_ptr<Player> _player)
{
	if (m_monsterStatus.hp <= 0)
		return;

}

