#include "pch.h"
#include "Nicky.h"
#include "NickyESkillState.h"
#include "NickyQSkillState.h"
#include "NickyRSkillState.h"
#include "NickyMoveState.h"
#include "NickyRunState.h"
#include "NickyWaitState.h"
#include "NickyWSkillState.h"
#include "FogOfWar.h"

Nicky::Nicky(shared_ptr<Shader> _defaultShader)
{
	m_defaultShader = _defaultShader;
	SetName(L"Nicky");
}

Nicky::~Nicky()
{

}

void Nicky::Start()
{
	InitNickyModel();
	InitNickyAnimation();
	InitNickyComponent();
	InitNickySkill();
	InitNickyStats();
	Super::Start();
}

void Nicky::Update()
{
	Super::Update();
}

void Nicky::LateUpdate()
{
	Super::LateUpdate();
}

void Nicky::FixedUpdate()
{
	Super::FixedUpdate();
}

void Nicky::OnCollision(shared_ptr<GameObject> _other)
{
}

void Nicky::OnCollisionEnter(shared_ptr<GameObject> _other)
{
}

void Nicky::OnCollisionExit(shared_ptr<GameObject> _other)
{
}

void Nicky::InitNickyModel()
{
	m_model = make_shared<Model>();
	m_model->ReadModel(L"Nicky/Nicky");
	m_model->ReadMaterial(L"Nicky/Nicky");
}

void Nicky::InitNickyAnimation()
{
	//대기
	m_model->ReadAnimation(L"Wait", L"Nicky/Nicky_Glove_Wait");

	//달리기
	m_model->ReadAnimation(L"Run", L"Nicky/Nicky_Glove_Run");

	//평타
	m_model->ReadAnimation(L"BaseAttack_01", L"Nicky/Nicky_Glove_Atk_01");
	m_model->ReadAnimation(L"BaseAttack_02", L"Nicky/Nicky_Glove_Atk_02");

	////Q
	m_model->ReadAnimation(L"Skill_01_Attack", L"Nicky/Nicky_Glove_Skill_01_Attack");
	m_model->ReadAnimation(L"Skill_01_Rush", L"Nicky/Nicky_Glove_Skill_01_Rush");
	m_model->ReadAnimation(L"Skill_01_End", L"Nicky/Nicky_Glove_Skill_01_End");
	//Q Charge
	m_model->ReadAnimation(L"Skill_01_Charge_Loop_Run", L"Nicky/Nicky_Glove_Skill_01_Charge_Loop_Run");
	m_model->ReadAnimation(L"Skill_01_Charge_Start_Run", L"Nicky/Nicky_Glove_Skill_01_Charge_Start_Run");
	m_model->ReadAnimation(L"Skill_01_Charge_Loop_Wait", L"Nicky/Nicky_Glove_Skill_01_Charge_Loop_Wait");
	m_model->ReadAnimation(L"Skill_01_Charge_Start_Wait", L"Nicky/Nicky_Glove_Skill_01_Charge_Start_Wait");

	//W
	m_model->ReadAnimation(L"Skill_02_Guard", L"Nicky/Nicky_Glove_Skill_02_Guard");
	m_model->ReadAnimation(L"Skill_02_Loop", L"Nicky/Nicky_Glove_Skill_02_Loop");

	//E
	m_model->ReadAnimation(L"Skill_03", L"Nicky/Nicky_Glove_Skill_03");

	//R
	m_model->ReadAnimation(L"Skill_04_Attack", L"Nicky/Nicky_Glove_Skill_04_Attack");
	m_model->ReadAnimation(L"Skill_04_Ready", L"Nicky/Nicky_Glove_Skill_04_Ready");
	m_model->ReadAnimation(L"Skill_04_Start", L"Nicky/Nicky_Glove_Skill_04_Start");

	AddComponent(make_shared<ModelAnimator>(m_defaultShader));
	{
		GetModelAnimator()->SetModel(m_model);
		GetModelAnimator()->SetPass(2);
	}
	//FSM 추가. 
	AddComponent(make_shared<AnimationStateMachine>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait, make_shared<NickyWaitState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Run, make_shared<NickyRunState>());

	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_2, make_shared<NickyWSkillState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_3, make_shared<NickyESkillState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_4, make_shared<NickyRSkillState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_1, make_shared<NickyQSkillState>());


	auto animator = GetModelAnimator();
	// 평타 시퀀스 (BaseAttack_01 -> BaseAttack_02)
	vector<wstring> baseAttackAnims = { L"BaseAttack_02", L"BaseAttack_01" };
	vector<float> baseAttackDurations = { 0.8f, 1.2f };
	animator->CreateSequence(L"BaseAttack_Sequence", baseAttackAnims, baseAttackDurations, false);

	// Q 스킬 시퀀스 (Skill_01_Attack -> Skill_01_Rush -> Skill_01_End)
	vector<wstring> skill1Anims = { L"Skill_01_Attack", L"Skill_01_Rush", L"Skill_01_End" };
	vector<float> skill1Durations = { 0.5f, 1.0f, 0.7f };
	animator->CreateSequence(L"Skill_1_Sequence", skill1Anims, skill1Durations, false);

	// W 스킬 시퀀스 (Skill_02_Guard -> Skill_02_Loop)
	vector<wstring> skill2Anims = { L"Skill_02_Guard" };
	animator->CreateSequence(L"Skill_2_Sequence", skill2Anims, false);

	// E 스킬 시퀀스 (Skill_03 단일)
	vector<wstring> skill3Anims = { L"Skill_03" };
	animator->CreateSequence(L"Skill_3_Sequence", skill3Anims, false);

	// R 스킬 시퀀스 (Skill_04_Ready -> Skill_04_Start -> Skill_04_Attack)
	vector<wstring> skill4Anims = { L"Skill_04_Ready", L"Skill_04_Start", L"Skill_01_Rush", L"Skill_04_Attack" };
	vector<float> skill4Durations;
	skill4Durations.push_back(animator->GetAnimationDuration(L"Skill_04_Ready"));
	skill4Durations.push_back(animator->GetAnimationDuration(L"Skill_04_Start"));
	skill4Durations.push_back(3.f);
	skill4Durations.push_back(animator->GetAnimationDuration(L"Skill_04_Attack"));
	animator->CreateSequence(L"Skill_4_Sequence", skill4Anims, skill4Durations, false);
}

void Nicky::InitNickyComponent()
{
	m_collider = make_shared<SphereCollider>();
	m_collider->SetOffset(Vec3(0, 1, 0));
	m_rigidbody = make_shared<Rigidbody>();
	m_navAgent = make_shared<NavMeshAgent>();

	AddComponent(m_collider);
	AddComponent(m_rigidbody);
	AddComponent(m_navAgent);
	AddComponent(make_shared<FogOfWar>());
}

void Nicky::InitNickySkill()
{
}

void Nicky::InitNickyStats()
{
	//기본 세팅. 
	SetHitAttack(32.f);
	SetDefense(55);
	SetMaxHP(1000);
	SetHP(1000);
	SetHealing(1.355);
	SetMaxStamina(600);
	SetStamina(600);
	SetHealingStamina(4.9);
	SetHitSpeed(0.5);
	SetMoveSpeed(3.5);


	//성장치 세팅. 
	m_growStatus.HitAttack = 4.2;
	m_growStatus.defense = 3.4;
	m_growStatus.hp = 95;
	m_growStatus.healing = 0.135;
	m_growStatus.stamina = 26;
	m_growStatus.healing_Stamina = 0.03;
}


void Nicky::Birth()
{
}

void Nicky::Death()
{
}

void Nicky::MakeItem()
{
}

void Nicky::MakeFood()
{
}
