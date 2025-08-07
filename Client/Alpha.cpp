#include "pch.h"
#include "Alpha.h"

#include "AlphaAppearAI.h"
#include "AlphaAttackAI.h"
#include "AlphaDeathAI.h"
#include "AlphaIdleAI.h"

#include "AlphaAppearState.h"
#include "AlphaAttack1State.h"
#include "AlphaDeathState.h"
#include "AlphaDyingState.h"
#include "AlphaSkill1State.h"
#include "AlphaWaitState.h"
#include "AlphaWalkState.h"

Alpha::Alpha(shared_ptr<Shader> _shader)
	: Super(_shader)
{
	SetName(L"Alpha");
	m_monsterState = MonsterState::APPEAR;
}

Alpha::~Alpha()
{

}

void Alpha::Start()
{
	Super::Start();

	InitAlphaModel();
	InitAlphaAnimation();
	InitAlphaComponent();
	InitAlphaAI();
	InitAlphaStats();
	UpdateState();

}

void Alpha::Update()
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

void Alpha::LateUpdate()
{
	Super::LateUpdate();
}

void Alpha::FixedUpdate()
{
	Super::FixedUpdate();
}

void Alpha::OnCollision(shared_ptr<GameObject> _other)
{
}

void Alpha::OnCollisionEnter(shared_ptr<GameObject> _other)
{
}

void Alpha::OnCollisionExit(shared_ptr<GameObject> _other)
{
}

void Alpha::UpdateState()
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

void Alpha::InitAlphaModel()
{
	m_model = make_shared<Model>();
	m_model->ReadModel(L"alpha/alpha_mesh");
	m_model->ReadMaterial(L"alpha/alpha_mesh");
}

void Alpha::InitAlphaAnimation()
{
	//Appear Wait
	m_model->ReadAnimation(L"Appear", L"alpha/alpha_appear_anim");
	m_model->ReadAnimation(L"Atk1", L"alpha/alpha_atk1_anim");
	m_model->ReadAnimation(L"Atk2", L"alpha/alpha_atk2_anim");
	m_model->ReadAnimation(L"Death", L"alpha/alpha_death_anim");
	m_model->ReadAnimation(L"Dying", L"alpha/alpha_dying_anim");
	m_model->ReadAnimation(L"Run", L"alpha/alpha_walk_anim");
	m_model->ReadAnimation(L"Wait", L"alpha/alpha_wait_anim");

	m_model->ReadAnimation(L"Skill1atk", L"alpha/alpha_skill1atk_anim");
	m_model->ReadAnimation(L"Skill1ready", L"alpha/alpha_skill1ready_anim");
	//m_model->ReadAnimation(L"Skill2", L"alpha/alpha_skill2_anim");

	AddComponent(make_shared<ModelAnimator>(m_defaultShader));
	{
		GetModelAnimator()->SetModel(m_model);
		GetModelAnimator()->SetPass(2);
	}

	//FSM 추가. 
	AddComponent(make_shared<AnimationStateMachine>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait, make_shared<AlphaWaitState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Appear, make_shared<AlphaAppearState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Move, make_shared<AlphaWalkState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::BaseAttack, make_shared<AlphaAttack1State>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_1, make_shared<AlphaSkill1State>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Dead, make_shared<AlphaDeathState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Dying, make_shared<AlphaDyingState>());


	auto animator = GetModelAnimator();

	// Wolf 등장 시퀀스. 
	vector<wstring> skill1 = { L"Skill1ready", L"Skill1atk" };
	animator->CreateSequence(L"Alpha_Skill1_Sequence", skill1, false);
}

void Alpha::InitAlphaComponent()
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

void Alpha::InitAlphaAI()
{
	auto sharedThis = dynamic_pointer_cast<Monster>(shared_from_this());

	auto appearAI = make_shared<AlphaAppearAI>(sharedThis);
	auto attackAI = make_shared<AlphaAttackAI>(sharedThis);
	auto deathAI = make_shared<AlphaDeathAI>(sharedThis);
	auto idleAI = make_shared<AlphaIdleAI>(sharedThis);

	m_AIMap[L"Appear"] = appearAI;
	m_AIMap[L"Attack"] = attackAI;
	m_AIMap[L"Death"] = deathAI;
	m_AIMap[L"Idle"] = idleAI;

	m_curAI = appearAI;
	m_curAI->Enter();
}

void Alpha::InitAlphaStats()
{
}
