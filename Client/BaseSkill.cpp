#include "pch.h"
#include "BaseSkill.h"
#include "Player.h"

BaseSkill::BaseSkill(shared_ptr<Player> _player)
	: m_playerObject(_player)
{

}

BaseSkill::~BaseSkill()
{

}

void BaseSkill::Update()
{

}

void BaseSkill::PlaySkill()
{

}

void BaseSkill::SkillEnd()
{
	m_skillcurCooldown = m_skillCooldown * (1 - m_playerObject->GetStatus().cooldownReduction);
}
