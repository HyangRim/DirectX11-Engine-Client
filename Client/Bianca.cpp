#include "pch.h"
#include "Bianca.h"
#include "BiancaWaitState.h"
#include "BiancaRunState.h"
#include "BiancaESkillState.h"
#include "BiancaQSkillState.h"
#include "BiancaRSkillState.h"
#include "FogOfWar.h"

#include "PlayerStateMachine.h"
#include "BiancaQSkill.h"
#include "BiancaWSkill.h"
#include "BiancaESkill.h"
#include "BiancaRSkill.h"

Bianca::Bianca(shared_ptr<Shader> _defaultShader)
{
	m_defaultShader = _defaultShader;
	SetName(L"Bianca");
} 

Bianca::~Bianca()
{
}

void Bianca::Start()
{
	InitBiancaModel();
	InitBiancaAnimation();
	InitBiancaComponent();
	InitBiancaSkill();
	InitBiancaStats();

	Super::Start();
}

void Bianca::Update()
{
	//if(INPUT->GetButtonDown(KEY_TYPE::))
	Super::Update();
}

void Bianca::LateUpdate()
{
	Super::LateUpdate();
}

void Bianca::FixedUpdate()
{
	Super::FixedUpdate();
}

void Bianca::OnCollision(shared_ptr<GameObject> _other)
{
}

void Bianca::OnCollisionEnter(shared_ptr<GameObject> _other)
{
}

void Bianca::OnCollisionExit(shared_ptr<GameObject> _other)
{
}

void Bianca::InitBiancaModel()
{
	m_model = make_shared<Model>();
	m_model->ReadModel(L"Bianca2/Bianca");
	m_model->ReadMaterial(L"Bianca2/Bianca");
}

void Bianca::InitBiancaAnimation()
{
	m_model->ReadAnimation(L"Wait", L"Bianca2/Bianca_wait");
	m_model->ReadAnimation(L"Run", L"Bianca2/Bianca_run");
	m_model->ReadAnimation(L"Skill_1", L"Bianca2/Bianca_skill1");
	m_model->ReadAnimation(L"Skill_3_1", L"Bianca2/Bianca_skill3-1");
	m_model->ReadAnimation(L"Skill_3_2", L"Bianca2/Bianca_skill3-2");
	m_model->ReadAnimation(L"Skill_3_3", L"Bianca2/Bianca_skill3-3");
	m_model->ReadAnimation(L"Skill_4_1", L"Bianca2/Bianca_skill4");
	m_model->ReadAnimation(L"Skill_4_2", L"Bianca2/Bianca_skill4-2");

	AddComponent(make_shared<ModelAnimator>(m_defaultShader));
	{
		GetModelAnimator()->SetModel(m_model);
		GetModelAnimator()->SetPass(2);
	}

	//FSM 추가. 
	AddComponent(make_shared<AnimationStateMachine>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait, make_shared<BiancaWaitState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Run, make_shared<BiancaRunState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_1, make_shared<BiancaQSkillState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_3, make_shared<BiancaESkillState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_4, make_shared<BiancaRSkillState>());

	auto animator = GetModelAnimator();
	vector<wstring> skill1Anims = { L"Skill_1" };
	animator->CreateSequence(L"Skill_1_Sequence", skill1Anims, false);

	// R 스킬 시퀀스 (Skill_04_Ready -> Skill_04_Start -> Skill_04_Attack)
	vector<wstring> skill4Anims = { L"Skill_4_1", L"Skill_4_2" };
	animator->CreateSequence(L"Skill_4_Sequence", skill4Anims, false);
}

void Bianca::InitBiancaComponent()
{
	m_collider = make_shared<SphereCollider>();
	m_collider->SetOffset(Vec3(0, 1, 0));
	m_rigidbody = make_shared<Rigidbody>();
	m_navAgent = make_shared<NavMeshAgent>();
	m_playerStateMachine = make_shared<PlayerStateMachine>(GetAnimationStateMachine(), 2);

	AddComponent(m_playerStateMachine);
	AddComponent(m_collider);
	AddComponent(m_rigidbody);
	AddComponent(m_navAgent);
	//AddComponent(make_shared<FogOfWar>());
}

void Bianca::InitBiancaSkill()
{
	m_skills[0] = make_shared<BiancaQSkill>(static_pointer_cast<Player>(shared_from_this()));
	m_skills[1] = make_shared<BiancaWSkill>(static_pointer_cast<Player>(shared_from_this()));
	m_skills[2] = make_shared<BiancaESkill>(static_pointer_cast<Player>(shared_from_this()));
	m_skills[3] = make_shared<BiancaRSkill>(static_pointer_cast<Player>(shared_from_this()));
}

void Bianca::InitBiancaStats()
{
	//기본 세팅. 
	SetHitAttack(31.f);
	SetDefense(51);
	SetMaxHP(870);
	SetHP(870);
	SetHealing(0.3);
	SetMaxStamina(580);
	SetStamina(580);
	SetHealingStamina(3.6);
	SetHitSpeed(0.4);
	SetMoveSpeed(3.4);


	//성장치 세팅. 
	m_growStatus.HitAttack = 4.4;
	m_growStatus.defense = 2.8;
	m_growStatus.hp = 82;
	m_growStatus.healing = 0.03;
	m_growStatus.stamina = 14;
	m_growStatus.healing_Stamina = 0.01;
}


void Bianca::Birth()
{
	//Birth Animation 관련. 
}

void Bianca::Death()
{

}

void Bianca::MakeItem()
{

}

void Bianca::MakeFood()
{

}
