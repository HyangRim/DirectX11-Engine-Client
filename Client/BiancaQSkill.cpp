#include "pch.h"
#include "BiancaQSkill.h"
#include "Player.h"
#include "BiancaQProjectile.h"

BiancaQSkill::BiancaQSkill(shared_ptr<Player> _player)
	: Super(_player)
{
	{
		m_skillImage = RESOURCES->GetOrAddTexture(L"BiancaR", L"..\\Resources\\Textures\\UI\\SkillIcon_1042200.png");
	}
	{
		m_Projectile = make_shared<BiancaQProjectile>();
		m_Projectile->SetName(L"Bianca_Q_Projectile");
		m_Projectile->SetActive(false);
	}
}

BiancaQSkill::~BiancaQSkill()
{
}

void BiancaQSkill::PlaySkill()
{
	POINT mousePos = INPUT->GetMousePos();
	::ScreenToClient(GAME->GetGameDesc().hWnd, &mousePos);

	XMVECTOR mouseWorldPos = ScreenToWorld(mousePos);

	XMVECTOR playerPos = m_playerObject->GetTransform()->GetPosition();
	XMVECTOR direction = XMVector3Normalize(mouseWorldPos - playerPos);
	XMVECTOR lengthVector = XMVector3Length(mouseWorldPos - playerPos);
	float Skilllength = min(m_skillRange, XMVectorGetX(lengthVector));


	//투사체 생성. 날리기.



	SkillEnd();
	
}

void BiancaQSkill::Update()
{

}
