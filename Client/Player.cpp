#include "pch.h"
#include "Player.h"
#include "BaseSkill.h"
#include "Item.h"
#include "EquipItem.h"

Player::Player()
{
}

Player::~Player()
{
	for (auto skill : m_skills) {
		if (skill != nullptr)
			skill.reset();
	}

	for (auto equipment : m_curEquipment) {
		if (equipment != nullptr)
			equipment.reset();
	}

	for (auto item : m_inventory) {
		if (item != nullptr)
			item.reset();
	}
}

void Player::Start()
{
	Super::Start();
}

void Player::Update()
{
	Super::Update();
	m_healingCoolTime += TIME->GetDeltaTime();

	if (m_healingCoolTime >= 0.25f) {
		SetHP(m_status.hp + m_status.healing);
		m_healingCoolTime = 0.f;
	}

	for (auto& skill : m_skills) {
		if (skill != nullptr) {
			skill->Update();
		}
	}
}

void Player::LateUpdate()
{
	Super::LateUpdate();
}

void Player::FixedUpdate()
{
	Super::FixedUpdate();
}

void Player::WearEquipment(shared_ptr<EquipItem> _item)
{
	//기존 Inventory에서 Equip빼주고. 
	//미리 빼주기 때문에 무조건 빔. 
	for (int idx = 0; idx < 10; ++idx) {
		if (_item == m_inventory[idx]) {
			m_inventory[idx] = nullptr;
			break;
		}
	}

	//Equip Index에 맞게 장착하기. 
	EquipmentType itemType = _item->GetEquipType();
	if (itemType == EquipmentType::HAT) {
		//비어있지 않다면. 
		if (m_curEquipment[1] != nullptr) {
			//장비 해제 후, 넣어주기. 
			TakeOffEquipment(1);
			m_curEquipment[1] = _item;
		}
		else {//비어있다면 그대로 넣어주기. 
			m_curEquipment[1] = _item;
		}
	}//하술 동일. 
	else if (itemType == EquipmentType::CLOTHES) {
		if (m_curEquipment[2] != nullptr) {
			TakeOffEquipment(2);
			m_curEquipment[2] = _item;
		}
		else {
			m_curEquipment[2] = _item;
		}
	}
	else if (itemType == EquipmentType::ARM) {
		if (m_curEquipment[3] != nullptr) {
			TakeOffEquipment(3);
			m_curEquipment[3] = _item;
		}
		else {
			m_curEquipment[3] = _item;
		}
	}
	else if (itemType == EquipmentType::LEGS) {
		if (m_curEquipment[4] != nullptr) {
			TakeOffEquipment(4);
			m_curEquipment[4] = _item;
		}
		else {
			m_curEquipment[4] = _item;
		}
	}
	else if (itemType == EquipmentType::WEAPON) {
		if (m_curEquipment[0] != nullptr) {
			TakeOffEquipment(0);
			m_curEquipment[0] = _item;
		}
		else {
			m_curEquipment[0] = _item;
		}
	}

	ApplyEquipStatus(_item->GetStatus());
}

void Player::TakeOffEquipment(int _index)
{
	if (_index < 0 || _index >= 5) return;
	if (m_curEquipment[_index] == nullptr) return;
	int emptyInventoryIDX = -1;
	for (int idx = 0; idx < 10; ++idx) {
		if (m_inventory[idx] == nullptr) {
			emptyInventoryIDX = idx;
			break;
		}
	}

	//비는 곳 없음. 
	if (emptyInventoryIDX == -1)
		return;
	ReleaseEquipStatus(m_curEquipment[_index]->GetStatus());
	m_inventory[emptyInventoryIDX] = move(m_curEquipment[_index]);
	m_curEquipment[_index] = nullptr;
}

void Player::LevelUp()
{
	if (m_status.curExp < m_status.curExpLimit)
		return;

	if (m_status.level >= 20)
		return;

	m_status.curExp -= m_status.curExpLimit;
	SetLevel(m_status.level + 1);
	m_status.curExpLimit += m_growStatus.ExpLimit;
	m_status.max_HP += m_growStatus.hp;
	m_status.hp += m_growStatus.hp;

	m_status.max_Stamina += m_growStatus.stamina;
	m_status.stamina += m_growStatus.stamina;

	m_status.hitAttack += m_growStatus.HitAttack;

	m_status.hitSpeed += m_growStatus.hitSpeed;
	m_status.defense += m_growStatus.defense;
	m_status.healing += m_growStatus.healing;
	m_status.healing_Stamina += m_growStatus.healing_Stamina;
}

void Player::ApplyEquipStatus(ItemStatus& _Equipstatus)
{
	float hpRatio = static_cast<float>(m_status.hp) / static_cast<float>(m_status.max_HP);
	float staminaRatio = static_cast<float>(m_status.hp) / static_cast<float>(m_status.max_HP);

	m_status.max_HP += _Equipstatus.hp;
	m_status.max_Stamina += _Equipstatus.stamina;
	m_status.hitAttack += _Equipstatus.hitattack;
	m_status.hitSpeed += _Equipstatus.hitSpeed;
	m_status.defense += _Equipstatus.defense;
	m_status.cooldownReduction += _Equipstatus.cooldownReduction;
	m_status.healing += _Equipstatus.healing;
	m_status.healing_Stamina += _Equipstatus.healing_Stamina;

	m_status.hp = m_status.max_HP * hpRatio;
	m_status.stamina = m_status.max_Stamina * staminaRatio;
}

void Player::ReleaseEquipStatus(ItemStatus& _Equipstatus)
{
	float hpRatio = static_cast<float>(m_status.hp) / static_cast<float>(m_status.max_HP);
	float staminaRatio = static_cast<float>(m_status.hp) / static_cast<float>(m_status.max_HP);

	m_status.max_HP -= _Equipstatus.hp;
	m_status.max_Stamina -= _Equipstatus.stamina;
	m_status.hitAttack -= _Equipstatus.hitattack;
	m_status.hitSpeed -= _Equipstatus.hitSpeed;
	m_status.defense -= _Equipstatus.defense;
	m_status.cooldownReduction -= _Equipstatus.cooldownReduction;
	m_status.healing -= _Equipstatus.healing;
	m_status.healing_Stamina -= _Equipstatus.healing_Stamina;

	m_status.hp = m_status.max_HP * hpRatio;
	m_status.stamina = m_status.max_Stamina * staminaRatio;
}

