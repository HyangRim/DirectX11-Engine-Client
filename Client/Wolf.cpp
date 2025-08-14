#include "pch.h"
#include "Wolf.h"
#include "ItemBox.h"

#include "WolfAppearState.h"
#include "WolfDeathState.h"
#include "WolfDyingState.h"
#include "WolfRunState.h"
#include "WolfWaitState.h"

#include "WolfAnimAppearState.h"
#include "WolfAnimDeathState.h"
#include "WolfAnimDyingState.h"
#include "WolfAnimRunState.h"
#include "WolfAnimWaitState.h"

#include "MonsterStateMachine.h"

Wolf::Wolf(shared_ptr<Shader> _shader)
	: Super(_shader)
{
	SetName(L"Wolf");
}

Wolf::~Wolf()
{
}

void Wolf::Start()
{
	InitWolfModel();
	InitWolfAnimation();
	InitWolfMSM();
	InitWolfComponent();
	//InitWolfAI();
	
	InitWolfStats();
	UpdateState();

	Super::Start();
}

void Wolf::Update()
{
	Super::Update();
}

void Wolf::LateUpdate()
{
	Super::LateUpdate();
}

void Wolf::FixedUpdate()
{
	Super::FixedUpdate();
}

void Wolf::OnCollision(shared_ptr<GameObject> _other)
{
}

void Wolf::OnCollisionEnter(shared_ptr<GameObject> _other)
{
}

void Wolf::OnCollisionExit(shared_ptr<GameObject> _other)
{
}


void Wolf::UpdateState()
{
}

void Wolf::InitWolfModel()
{
	m_model = make_shared<Model>();
	m_model->ReadModel(L"wolf/wolf_mesh");
	m_model->ReadMaterial(L"wolf/wolf_mesh");
}

void Wolf::InitWolfAnimation()
{
	m_model->ReadAnimation(L"Appear", L"wolf/wolf_appear_anim");
	m_model->ReadAnimation(L"AppearWait", L"wolf/wolf_appearwait_anim");
	/*m_model->ReadAnimation(L"Atk1", L"wolf/wolf_atk1_anim");
	m_model->ReadAnimation(L"Atk2", L"wolf/wolf_atk2_anim");*/
	m_model->ReadAnimation(L"Death", L"wolf/wolf_death_anim");
	m_model->ReadAnimation(L"Dying", L"wolf/wolf_dying_anim");
	m_model->ReadAnimation(L"Run", L"wolf/wolf_run_anim");
	//m_model->ReadAnimation(L"Skill", L"wolf/wolf_skill_anim");
	m_model->ReadAnimation(L"Wait", L"wolf/wolf_wait_anim");

	AddComponent(make_shared<ModelAnimator>(m_defaultShader));
	{
		GetModelAnimator()->SetModel(m_model);
		GetModelAnimator()->SetPass(2);
	}

	//FSM 추가. 
	m_animationStateMachine = make_shared<AnimationStateMachine>(AnimationStateType::Appear);
	AddComponent(m_animationStateMachine);
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait,			make_shared<WolfAnimWaitState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Appear,		make_shared<WolfAnimAppearState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Move,			make_shared<WolfAnimRunState>());
	//GetAnimationStateMachine()->RegisterState(AnimationStateType::BaseAttack,	make_shared<WolfAttack1State>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Death,		make_shared<WolfAnimDeathState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Dying,		make_shared<WolfAnimDyingState>());

	auto animator = GetModelAnimator();

	// Wolf 등장 시퀀스. 
	vector<wstring> appearAnims = { L"Appear", L"AppearWait"};
	vector<float> appearAnimsDurations;
	appearAnimsDurations.push_back(animator->GetAnimationDuration(L"Appear")); 
	appearAnimsDurations.push_back(animator->GetAnimationDuration(L"AppearWait"));
	animator->CreateSequence(L"Wolf_Appear_Sequence", appearAnims, appearAnimsDurations, false);

	// Wolf 사망하는 시퀀스. 
	vector<wstring> deadAnims = { L"Death" };
	vector<float> deadAnimsDurations;
	deadAnimsDurations.push_back(animator->GetAnimationDuration(L"Death"));
	animator->CreateSequence(L"Wolf_death_Sequence", deadAnims, deadAnimsDurations, false);

	// Wolf 완전 죽어서 시체인 상태 시퀀스. 
	vector<wstring> dyingAnims = { L"Dying" };
	vector<float> dyingAnimsDurations;
	dyingAnimsDurations.push_back(animator->GetAnimationDuration(L"Dying"));
	animator->CreateSequence(L"Wolf_dying_Sequence", dyingAnims, dyingAnimsDurations, true);
}

void Wolf::InitWolfComponent()
{
	m_collider = make_shared<SphereCollider>();
	m_collider->SetOffset(Vec3(0, 1, 0));
	m_collider->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	m_collider->SetVisible(false);
	m_rigidbody = make_shared<Rigidbody>();
	m_navAgent = make_shared<NavMeshAgent>();
	m_itembox = make_shared<ItemBox>();


	AddComponent(m_collider);
	AddComponent(m_rigidbody);
	AddComponent(m_navAgent);
	AddComponent(m_itembox);
}

void Wolf::InitWolfAI()
{
	/*auto sharedThis = dynamic_pointer_cast<Monster>(shared_from_this());

	auto appearAI = make_shared<WolfAppearAI>(sharedThis);
	auto attackAI = make_shared<WolfAttackAI>(sharedThis);
	auto deathAI = make_shared<WolfDeathAI>(sharedThis);
	auto idleAI = make_shared<WolfIdleAI>(sharedThis);

	m_AIMap[L"Appear"] = appearAI;
	m_AIMap[L"Attack"] = attackAI;
	m_AIMap[L"Death"] = deathAI;
	m_AIMap[L"Idle"] = idleAI;

	m_curAI = appearAI;
	m_curAI->Enter();*/
}

void Wolf::InitWolfMSM()
{
	m_monsterStateMachine = make_shared<MonsterStateMachine>(m_animationStateMachine);
	AddComponent(m_monsterStateMachine);

	m_monsterStateMachine->RegisterState(MonsterStateType::Wait, make_shared<WolfWaitState>());
	m_monsterStateMachine->RegisterState(MonsterStateType::Appear, make_shared<WolfAppearState>());
	m_monsterStateMachine->RegisterState(MonsterStateType::Move, make_shared<WolfRunState>());
	m_monsterStateMachine->RegisterState(MonsterStateType::Death, make_shared<WolfDeathState>());
	m_monsterStateMachine->RegisterState(MonsterStateType::Dying, make_shared<WolfDyingState>());
}

void Wolf::InitWolfStats()
{

}
