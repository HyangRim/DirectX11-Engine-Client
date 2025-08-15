#include "pch.h"
#include "Monster.h"
#include "AI.h"
#include "Item.h"
#include "Player.h"
#include "MonsterStateMachine.h"
#include "MonsterState.h"

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
	Super::Update();


	if (m_isStun > 0.f) {
		m_isStun -= DT;
	}

	if (IsStun())
		return;


	if (m_curAI != nullptr) {
		m_curAI->Update();
	}
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

void Monster::Damaged(DamageInfo _damage)
{
	m_targetPlayer = static_pointer_cast<Player>(_damage.attacker);


	MonsterStatus info = GetMonsterStatus();

	int32 baseAttack = _damage.damage * 100;
	int32 baseDefense = 100;

	int32 finalDamage = baseAttack / baseDefense;

	int32 monsterHP = info.hp;
	monsterHP -= finalDamage;



	if (monsterHP <= 0) {
		//사망 애니메이션으로. 
		GetMonsterStateMachine()->ChangeState(MonsterStateType::Death);
		m_isStun = true;
	}

	if (_damage.stunTime > 0.f) {
		m_isStun = max(m_isStun, _damage.stunTime);
		SetType(OBJECTTYPE::ITEMBOX);
	}

	SetHP(monsterHP);
}

