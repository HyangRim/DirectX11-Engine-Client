#include "pch.h"
#include "Player.h"
#include "NickyESkill.h"
#include "NickyERange.h"
#include "PlayerStateMachine.h"

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

		m_skillRange->GetTransform()->SetScale(Vec3(0.01f, 0.01f, 0.01f));
		m_skillRange->GetTransform()->SetLocalRotation(Vec3(90.f, 0.f, 0.f));

		m_skillRange->AddComponent(make_shared<SphereCollider>());
		m_skillRange->AddComponent(make_shared<Rigidbody>());
		m_skillRange->GetCollider()->SetOffset(Vec3(0.f, 1.f, 0.f));
		m_skillRange->GetRigidbody()->SetStatic(true);

		m_skillRange->AddComponent(make_shared<ModelRenderer>(m_shader));
		{
			m_skillRange->GetModelRenderer()->SetModel(m1);
			m_skillRange->GetModelRenderer()->SetPass(1);
		}

		m_skillRange->SetActive(false);
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
}

void NickyESkill::Update()
{
	Vec3 playerPos = m_playerObject->GetTransform()->GetPosition();
	
	m_skillRange->GetTransform()->SetPosition(playerPos);
	
	PlayerStateType curState = m_playerObject->GetPlayerStateMachine()->GetCurrentState();
	if (curState != PlayerStateType::Skill_3)
		m_skillRange->SetActive(false);
}
