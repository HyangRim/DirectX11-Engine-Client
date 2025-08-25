#include "pch.h"
#include "NickyWSkill.h"

#include "Player.h"
#include "PlayerStateMachine.h"
#include "AnimationStateMachine.h"

NickyWSkill::NickyWSkill(shared_ptr<Player> _player)
	: Super(_player, 1)
	, m_player(_player)
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
