#include "pch.h"
#include "BiancaQSkill.h"
#include "Player.h"
#include "BiancaQProjectile.h"
#include "BiancaQCone.h"

BiancaQSkill::BiancaQSkill(shared_ptr<Player> _player)
	: Super(_player)
{
	{
		m_skillImage = RESOURCES->GetOrAddTexture(L"BiancaR", L"..\\Resources\\Textures\\UI\\SkillIcon_1042200.png");
	}
	{
		m_Projectile = make_unique<BiancaQProjectile>();
		m_Projectile->SetName(L"Bianca_Q_Projectile");
		m_Projectile->SetActive(false);
		m_Projectile->AddComponent(make_shared<MeshRenderer>());
		m_Projectile->AddComponent(make_shared<SphereCollider>());
		{
			auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
			m_Projectile->GetMeshRenderer()->SetMesh(mesh);
			m_Projectile->GetMeshRenderer()->SetPass(0);
			m_Projectile->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"default"));
		}
	}

	{
		m_Cone = make_unique<BiancaQCone>();
		m_Cone->SetName(L"Bianca_Q_Cone");
		m_Cone->SetActive(false);
		m_Cone->AddComponent(make_shared<MeshRenderer>());
		m_Cone->AddComponent(make_shared<AABBBoxCollider>());
		{
			auto mesh = RESOURCES->Get<Mesh>(L"Cone");
			m_Cone->GetMeshRenderer()->SetMesh(mesh);
			m_Cone->GetMeshRenderer()->SetPass(0);
			m_Cone->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"default"));
		}
	}
}

BiancaQSkill::~BiancaQSkill()
{
}

void BiancaQSkill::PlaySkill()
{
	if (m_skillcurCooldown >= 0.f) return;
	cout << "Bianca Q Skill 시작 !!! \n";


	POINT mousePos = INPUT->GetMousePos();
	::ScreenToClient(GAME->GetGameDesc().hWnd, &mousePos);

	XMVECTOR mouseWorldPos = ScreenToWorld(mousePos);

	XMVECTOR playerPos = m_playerObject->GetTransform()->GetPosition();
	XMVECTOR direction = XMVector3Normalize(mouseWorldPos - playerPos);
	XMVECTOR lengthVector = XMVector3Length(mouseWorldPos - playerPos);
	float Skilllength = min(m_skillRange, XMVectorGetX(lengthVector));

	
	//투사체 생성. 날리기.


	SOUND->PlaySound(L"Bianca_Skill01.wav", 1, 0.5f);


	SkillEnd();
	
}

void BiancaQSkill::Update()
{
	if (m_Projectile->GetArrive()) {
		//m_Projectile이 도착한 경우. Arrive를 False로 바꾸고. 
		m_Projectile->SetArrive(false);

		//그 자리에 Cone 배치.
		Vec3 ProjectilePos = m_Projectile->GetTransform()->GetLocalPosition();
		ProjectilePos.y -= 3.f;

		m_Cone->GetTransform()->SetLocalPosition(ProjectilePos);
		m_Cone->SetActive(true);
	}
}
