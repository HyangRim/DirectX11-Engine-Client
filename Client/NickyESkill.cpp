#include "pch.h"
#include "Player.h"
#include "NickyESkill.h"

NickyESkill::NickyESkill(shared_ptr<Player> _player)
	: Super(_player)
{
	m_shader = _player->GetShader();
	m_skillCooldown = 10.f;
	m_skillName = L"강력한 펀치";
	m_skillDesc = L"니키가 강력한 펀치로 전방의 적에게 스킬 피해를 입히고 2초 동안 이동 속도를 35% 감소시킵니다.";
	m_skillLevel = 1;


	{
		m_skillImage = RESOURCES->GetOrAddTexture(L"NickyE", L"..\\Resources\\Textures\\UI\\SkillIcon_1033400");
	}
}

NickyESkill::~NickyESkill()
{

}

void NickyESkill::PlaySkill()
{

}

void NickyESkill::Update()
{

}
