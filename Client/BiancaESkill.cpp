#include "pch.h"
#include "BiancaESkill.h"

BiancaESkill::BiancaESkill(shared_ptr<Player> _player)
	: Super(_player)
{
	{
		m_skillImage = RESOURCES->GetOrAddTexture(L"BiancaE", L"..\\Resources\\Textures\\UI\\SkillIcon_1042400.png");
	}
}

BiancaESkill::~BiancaESkill()
{
}

void BiancaESkill::PlaySkill()
{
}

void BiancaESkill::Update()
{
}
