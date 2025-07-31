#include "pch.h"
#include "BiancaQSkill.h"

BiancaQSkill::BiancaQSkill(shared_ptr<Player> _player)
	: Super(_player)
{
	{
		m_skillImage = RESOURCES->GetOrAddTexture(L"BiancaR", L"..\\Resources\\Textures\\UI\\SkillIcon_1042200.png");
	}
}

BiancaQSkill::~BiancaQSkill()
{
}

void BiancaQSkill::PlaySkill()
{
}

void BiancaQSkill::Update()
{
}
