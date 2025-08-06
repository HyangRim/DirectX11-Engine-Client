#include "pch.h"
#include "Wolf.h"

#include "WolfAppearState.h"
#include "WolfAttack1State.h"
#include "WolfDeathState.h"
#include "WolfDyingState.h"
#include "WolfRunState.h"
#include "WolfWaitState.h"

#include "AI.h"
#include "WolfAppearAI.h"
#include "WolfAttackAI.h"
#include "WolfDeathAI.h"
#include "WolfIdleAI.h"

Wolf::Wolf(shared_ptr<Shader> _shader)
	: Super(_shader)
{
	SetName(L"Wolf");
	m_monsterState = MonsterState::APPEAR;
}

Wolf::~Wolf()
{
}

void Wolf::Start()
{
	InitWolfModel();
	InitWolfAnimation();
	InitWolfComponent();
	InitWolfAI();
	InitWolfStats();
	UpdateState();

	Super::Start();
}

void Wolf::Update()
{

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_5)) {
		ChangeState(L"Appear");
	}
	else if (INPUT->GetButtonDown(KEY_TYPE::KEY_6)) {
		ChangeState(L"Idle");
	}
	else if (INPUT->GetButtonDown(KEY_TYPE::KEY_7)) {
		ChangeState(L"Attack");
	}
	else if (INPUT->GetButtonDown(KEY_TYPE::KEY_8)) {
		ChangeState(L"Death");
	}
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
	if (m_monsterState == MonsterState::IDLE) {

	}
	else if (m_monsterState == MonsterState::RUN) {

	}
	else if (m_monsterState == MonsterState::ATTACK) {

	}
	else if (m_monsterState == MonsterState::DIE) {

	}
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
	m_model->ReadAnimation(L"Atk1", L"wolf/wolf_atk1_anim");
	m_model->ReadAnimation(L"Atk2", L"wolf/wolf_atk2_anim");
	m_model->ReadAnimation(L"Death", L"wolf/wolf_death_anim");
	m_model->ReadAnimation(L"Dying", L"wolf/wolf_dying_anim");
	m_model->ReadAnimation(L"Run", L"wolf/wolf_run_anim");
	m_model->ReadAnimation(L"Skill", L"wolf/wolf_skill_anim");
	m_model->ReadAnimation(L"Wait", L"wolf/wolf_wait_anim");

	AddComponent(make_shared<ModelAnimator>(m_defaultShader));
	{
		GetModelAnimator()->SetModel(m_model);
		GetModelAnimator()->SetPass(2);
	}

	//FSM Ãß°¡. 
	AddComponent(make_shared<AnimationStateMachine>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait, make_shared<WolfWaitState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Appear, make_shared<WolfAppearState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Move, make_shared<WolfRunState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::BaseAttack, make_shared<WolfAttack1State>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Dead, make_shared<WolfDeathState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Dying, make_shared<WolfDyingState>());

	auto animator = GetModelAnimator();

	// Wolf µîÀå ½ÃÄö½º. 
	vector<wstring> appearAnims = { L"Appear", L"AppearWait"};
	animator->CreateSequence(L"Wolf_Appear_Sequence", appearAnims, false);

	// Wolf »ç¸Á ½ÃÄö½º. 
	vector<wstring> deadAnims = { L"Dying" };
	animator->CreateSequence(L"Wolf_death_Sequence", deadAnims, false);

	// Wolf »ç¸Á ½ÃÄö½º. 
	vector<wstring> dyingAnims = { L"Death" };
	animator->CreateSequence(L"Wolf_dying_Sequence", dyingAnims, true);
}

void Wolf::InitWolfComponent()
{
	m_collider = make_shared<SphereCollider>();
	m_collider->SetOffset(Vec3(0, 1, 0));
	m_collider->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	m_collider->SetVisible(false);
	m_rigidbody = make_shared<Rigidbody>();
	m_navAgent = make_shared<NavMeshAgent>();

	AddComponent(m_collider);
	AddComponent(m_rigidbody);
	AddComponent(m_navAgent);
}

void Wolf::InitWolfAI()
{
	auto sharedThis = dynamic_pointer_cast<Monster>(shared_from_this());

	auto appearAI = make_shared<WolfAppearAI>(sharedThis);
	auto attackAI = make_shared<WolfAttackAI>(sharedThis);
	auto deathAI = make_shared<WolfDeathAI>(sharedThis);
	auto idleAI = make_shared<WolfIdleAI>(sharedThis);

	m_AIMap[L"Appear"] = appearAI;
	m_AIMap[L"Attack"] = attackAI;
	m_AIMap[L"Death"] = deathAI;
	m_AIMap[L"Idle"] = idleAI;

	m_curAI = appearAI;
	m_curAI->Enter();
}

void Wolf::InitWolfStats()
{

}
