#include "pch.h"
#include "NickyWSkill.h"

NickyWSkill::NickyWSkill(shared_ptr<Player> _player)
	: Super(_player, 1)
{
	m_skillCooldown = 5.f;
	m_skillImage = RESOURCES->GetOrAddTexture(L"NickyW", L"..\\Resources\\Textures\\UI\\SkillIcon\\SkillIcon_1033300.png");
}

NickyWSkill::~NickyWSkill()
{
}

void NickyWSkill::PlaySkill()
{
}

void NickyWSkill::Update()
{
}
