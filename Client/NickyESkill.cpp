#include "pch.h"
#include "Player.h"
#include "NickyESkill.h"
#include "NickyERange.h"
#include "PlayerStateMachine.h"

#include "BaseCollider.h"
#include "AABBBoxCollider.h"

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
	{

		shared_ptr<Model> m1 = make_shared<Model>();

		m1->ReadModel(L"Nicky/NickyESkill_Mesh");
		m1->ReadMaterial(L"Nicky/NickyESkill_Mesh");

		m_skillRange = make_shared<NickyERange>();
		m_skillRange->SetName(L"Nicky_E_Range");
		m_skillRange->SetActive(false);

		m_skillRange->GetTransform()->SetScale(Vec3(0.01f, 0.01f, 0.01f));
		m_skillRange->GetTransform()->SetLocalRotation(Vec3(90.f, 0.f, 0.f));

		m_skillRange->AddComponent(make_shared<SphereCollider>());

		m_skillRange->GetCollider()->SetOffset(Vec3(0.f, 1.f, -3.f));
		m_skillRange->GetCollider()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
		
		m_skillRange->AddComponent(make_shared<ModelRenderer>(m_shader));
		{
			m_skillRange->GetModelRenderer()->SetModel(m1);
			m_skillRange->GetModelRenderer()->SetPass(1);
		}

		
		m_skillRange->GetTransform()->SetParent(m_playerObject->GetTransform());
		CURSCENE->Add(m_skillRange);
	}
}

NickyESkill::~NickyESkill()
{

}

void NickyESkill::PlaySkill()
{
	cout << "Nicky E Skill 시작 !!! \n";

	m_skillRange->SetActive(true);
	m_bskillStart = true;

	//SOUND->PlaySound(L"Nicky/Nicky_Skill03.wav", 1, 0.5f);
}

void NickyESkill::Update()
{
	if (m_bskillStart)
	{
		m_duration += DT;
		//cout << "Duration : " << m_duration << endl;
		if (m_duration >= 0.45f)
		{
			//SOUND->PlaySound(L"Nicky/Nicky_Skill03.wav", 1, 0.5f);
			m_duration = 0.f;
		}
	}

	Vec3 playerPos = m_playerObject->GetTransform()->GetPosition();
	Vec3 playerRot = m_playerObject->GetTransform()->GetRotation();

	m_skillRange->GetTransform()->SetPosition(playerPos);
	
	 // Collider 오프셋을 플레이어 회전에 따라 계산
	if (m_skillRange->GetCollider())
	{
		// 플레이어 회전 행렬 생성
		Matrix rotationMatrix = Matrix::CreateRotationY(XMConvertToRadians(playerRot.y));
		rotationMatrix *= Matrix::CreateRotationX(XMConvertToRadians(playerRot.x));
		rotationMatrix *= Matrix::CreateRotationZ(XMConvertToRadians(playerRot.z));

		// 기본 오프셋 (0, 1, -3)을 회전시킴
		Vec3 baseOffset = Vec3(0.f, 1.f, -3.f);
		Vec3 rotatedOffset = Vec3::Transform(baseOffset, rotationMatrix);

		// Collider의 오프셋 업데이트
		m_skillRange->GetCollider()->SetOffset(rotatedOffset);
	}

	PlayerStateType curState = m_playerObject->GetPlayerStateMachine()->GetCurrentState();
	if (curState != PlayerStateType::Skill_3)
	{
		m_skillRange->SetActive(false);
		m_duration = 0.f;
		m_bskillStart = false;
	}
}
