#include "pch.h"
#include "ISkillExecutor.h"

#include "Nicky.h"


#include "NickyAnimQState.h"
#include "NickyAnimWState.h"
#include "NickyAnimEState.h"
#include "NickyAnimRState.h"
#include "NickyAnimRunState.h"
#include "NickyAnimWaitState.h"
#include "NickyAnimCraftState.h"

#include "NickyQState.h"
#include "NickyWState.h"
#include "NickyEState.h"
#include "NickyRState.h"
#include "NickyRunState.h"
#include "NickyWaitState.h"
#include "NickyCraftState.h"


#include "FogOfWar.h"

#include "PlayerStateMachine.h"

#include "NickyQSkill.h"
#include "NickyWSkill.h"
#include "NickyESkill.h"
#include "NickyRSkill.h"

#include "ModelMesh.h"

#include "PlayerInterface.h"

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
	InitNickyPSM();
	InitNickyComponent();
	
	InitNickySkill();
	InitNickyStats();
	Super::Start();
}

void Nicky::Update()
{
	Super::Update();

	if (isStun()) {
		GetNavMeshAgent()->Stop();
		return;
	}
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

	//제작 모션
	m_model->ReadAnimation(L"Craft", L"Nicky/Nicky_Craft");


	AddComponent(make_shared<ModelAnimator>(m_defaultShader));
	{
		GetModelAnimator()->SetModel(m_model);
		GetModelAnimator()->SetPass(2);
	}
	//FSM 추가. 
	AddComponent(make_shared<AnimationStateMachine>(AnimationStateType::Wait));
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait,		make_shared<NickyAnimWaitState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Run,		make_shared<NickyAnimRunState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_2,	make_shared<NickyAnimWState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_3,	make_shared<NickyAnimEState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_4,	make_shared<NickyAnimRState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_1,	make_shared<NickyAnimQState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Craft,	make_shared<NickyAnimCraftState>());

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

	// 제작모션
	vector<wstring> craftMotion = { L"Craft" };
	animator->CreateSequence(L"Craft_Sequence", craftMotion, false);
}

void Nicky::InitNickyPSM()
{
	m_playerStateMachine = make_shared<PlayerStateMachine>(GetAnimationStateMachine(), 8, 15, 1);

	auto self = static_pointer_cast<Player>(shared_from_this());
	m_playerInterface = make_shared<PlayerInterface>(self);

	m_playerStateMachine->SetPlayerInterface(m_playerInterface);

	m_playerStateMachine->RegisterState(PlayerStateType::Run,		make_shared<NickyRunState>());
	m_playerStateMachine->RegisterState(PlayerStateType::Wait,		make_shared<NickyWaitState>());

	m_playerStateMachine->RegisterState(PlayerStateType::Skill_1,	make_shared<NickyQState>(GetModelAnimator()));
	m_playerStateMachine->RegisterState(PlayerStateType::Skill_2,	make_shared<NickyWState>(GetModelAnimator()));
	m_playerStateMachine->RegisterState(PlayerStateType::Skill_3,	make_shared<NickyEState>(GetModelAnimator()));
	m_playerStateMachine->RegisterState(PlayerStateType::Skill_4,	make_shared<NickyRState>(GetModelAnimator()));
	m_playerStateMachine->RegisterState(PlayerStateType::Craft,		make_shared<NickyCraftState>(GetModelAnimator()));

	m_playerStateMachine->OnSkillUsed += [this](int skillIndex, shared_ptr<GameObject> target) {
		if (skillIndex >= 0 && skillIndex < (int)m_skills.size() && m_skills[skillIndex])
		{
			// R 스킬(인덱스 3)인 경우 타겟 설정
			if (skillIndex == 3 && target)
			{
				// NickyRSkill에 타겟 설정
				static_cast<NickyRSkill*>(m_skills[skillIndex].get())->SetTarget(target);
			}
			m_skills[skillIndex]->PlaySkill();
		}
		};
}

void Nicky::InitNickyComponent()
{
	m_collider = make_shared<SphereCollider>();
	m_collider->SetOffset(Vec3(0, 1, 0));
	m_collider->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	m_rigidbody = make_shared<Rigidbody>();
	m_navAgent = make_shared<NavMeshAgent>();
	
	AddComponent(m_collider);
	AddComponent(m_rigidbody);
	AddComponent(m_navAgent);
	AddComponent(m_playerStateMachine);
	//AddComponent(make_shared<FogOfWar>());

	////PlayerStateMachine 객체가 준비된 이후에 Delegate 등록
	//m_playerStateMachine->OnSkillUsed += [this](int skillIndex) {
	//	if (skillIndex >= 0 && skillIndex < (int)m_skills.size() && m_skills[skillIndex])
	//	{
	//		m_skills[skillIndex]->PlaySkill();
	//	}
	//};
	

}

void Nicky::InitNickySkill()
{
	m_skills[0] = make_unique<NickyQSkill>(static_pointer_cast<Player>(shared_from_this()));
	m_skills[1] = make_unique<NickyWSkill>(static_pointer_cast<Player>(shared_from_this()));
	m_skills[2] = make_unique<NickyESkill>(static_pointer_cast<Player>(shared_from_this()));
	m_skills[3] = make_unique<NickyRSkill>(static_pointer_cast<Player>(shared_from_this()));
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
	GetNavMeshAgent()->SetSpeed(3.5f);


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
