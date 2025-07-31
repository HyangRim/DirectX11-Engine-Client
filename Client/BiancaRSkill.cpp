#include "pch.h"
#include "BiancaRSkill.h"

BiancaRSkill::BiancaRSkill(shared_ptr<Player> _player)
	: Super(_player)
{

	{
		m_skillImage = RESOURCES->GetOrAddTexture(L"BiancaR", L"..\\Resources\\Textures\\UI\\SkillIcon_1042500.png");
	}
}

BiancaRSkill::~BiancaRSkill()
{
}

void BiancaRSkill::PlaySkill()
{
}

void BiancaRSkill::Update()
{
}
