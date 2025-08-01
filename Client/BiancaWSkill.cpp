#include "pch.h"
#include "Player.h"
#include "BiancaWSkill.h"

BiancaWSkill::BiancaWSkill(shared_ptr<Player> _player)
	: Super(_player)
{
	//Bianca Coffin Object
	m_shader = _player->GetShader();
	m_skillCooldown = 10.f;
	m_skillName = L"짧은 안식";
	m_skillDesc = L"비앙카가 정신 집중을 하며 최대 3초 동안 관속으로 잠시 몸을 피합니다.";
	m_skillLevel = 1;
	//Coffin 모델 생성. 
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"BiancaCoffin", L"Bianca/BiancaCoffin_mesh");
		m2->ReadMaterial(L"Bianca/BiancaCoffin_mesh");

		auto obj = make_shared<GameObject>();
		obj->SetName(L"Bianca_Coffin");
		obj->GetTransform()->SetParent(_player->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(0, 0, 0));
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetActive(false);
		obj->AddComponent(make_shared<ModelRenderer>(m_shader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		m_coffin = obj;
		CURSCENE->Add(obj);
	}
	

	{
		m_skillImage = RESOURCES->GetOrAddTexture(L"BiancaW", L"..\\Resources\\Textures\\UI\\SkillIcon_1042300.png");
	}
}

BiancaWSkill::~BiancaWSkill()
{
}

void BiancaWSkill::PlaySkill()
{
	cout << "Play BiancaW Skill!\n";
	if (m_isPlaying) {
		//너무 빠르게 다시 눌러 해제되는 것 방지. 
		if (m_repeatKey < 0.25f)
			return;
		//이미 실행중일 경우 -> W스킬 끝내기. 
		//스킬 종료. 
		m_coffin->SetActive(false);
		m_isPlaying = false;

		PlayerStatus status = m_playerObject->GetStatus();
		m_playerObject->SetDefense(status.defense - 50);
		m_repeatKey = 0.f;
		SkillEnd();
	}
	else if(m_isPlaying == false && m_skillcurCooldown <= 0){
		//실행중이지 않으며, 남아있는 쿨타임이 없을때. 
		//다시 말해 스킬 실행. 
		m_coffin->SetActive(true);
		m_isPlaying = true;

		PlayerStatus status = m_playerObject->GetStatus();
		m_playerObject->SetDefense(status.defense + 50);
	}
}

void BiancaWSkill::Update()
{
	static float debugTimer = 0.f;

	debugTimer += DT;
	if (debugTimer > 1.f) {
		if (m_coffin != nullptr) {
			cout << "coffin 준비";
		}
		if (m_coffin->GetActive()) {
			cout << " Vivible\n";
		}
		else
			cout << " Disable\n";
		debugTimer = 0.f;
	}
	if (m_isPlaying) {
		m_elapsedTime += DT;
		m_repeatKey += DT;
		if (m_elapsedTime >= m_duration) {
			PlaySkill();
		}
	}
	else {
		m_skillcurCooldown -= DT;
	}
}
