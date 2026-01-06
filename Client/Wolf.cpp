#include "pch.h"
#include "Wolf.h"
#include "ItemBox.h"

#include "WolfAppearState.h"
#include "WolfDeathState.h"
#include "WolfDyingState.h"
#include "WolfRunState.h"
#include "WolfWaitState.h"
#include "WolfTraceState.h"
#include "WolfAttackState.h"

#include "WolfAnimAppearState.h"
#include "WolfAnimDeathState.h"
#include "WolfAnimDyingState.h"
#include "WolfAnimRunState.h"
#include "WolfAnimWaitState.h"
#include "WolfAnimTraceState.h"
#include "WolfAnimAttackState.h"

#include "WolfBaseAttack.h"
#include "WolfTrace.h"


#include "MonsterStateMachine.h"
#include "MonsterInterface.h"


#include "SkillObject.h"
#include "Player.h"


#include "BehaviorTree.h"

Wolf::Wolf(shared_ptr<Shader> _shader)
	: Super(_shader)
{
	SetName(L"Wolf");
	m_itembox = make_shared<ItemBox>();
}

Wolf::~Wolf()
{

}

void Wolf::Start()
{
	InitWolfModel();
	InitWolfAnimation();
	//InitWolfMSM();
	InitWolfComponent();
	//InitWolfAI();
	
	InitBehaviorTree();

	InitWolfStats();
	UpdateState();

	Super::Start();
}

void Wolf::Update()
{
	m_behaviorTree->Update();
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
	//shared_ptr<GameObject> chaseTarget = nullptr;
	//// 만약 _other가 Player라면 (직접 충돌)
	//if (dynamic_pointer_cast<Player>(_other)) {
	//	chaseTarget = _other;
	//}
	//// 만약 _other가 스킬 오브젝트라면, owner를 찾아서 Player를 추적
	//else if (auto skillObj = dynamic_pointer_cast<SkillObject>(_other)) {
	//	if (skillObj->GetOwner()) {
	//		chaseTarget = skillObj->GetOwner();
	//	}
	//}

	//// 기타 예외 (추가 오브젝트 타입들은 필요시 확장)
	//if (chaseTarget) {
	//	//static_pointer_cast<WolfTraceState>(m_monsterStateMachine->GetState(MonsterStateType::Trace))->SetOtherObject(chaseTarget);
	//	//static_pointer_cast<WolfAttackState>(m_monsterStateMachine->GetState(MonsterStateType::Attack))->SetOtherObject(chaseTarget);
	//	
	//	GetComponent<WolfBaseAttack>()->SetTarget(chaseTarget);
	//	
	//	//m_monsterStateMachine->ChangeState(MonsterStateType::Trace);
	//	//m_animationStateMachine->ChangeState(AnimationStateType::Trace);
	//}

	// 피격 시 타겟 설정은 Monster::Damaged에서 처리하므로
	// 단순히 피격 플래그만 설정
	if (_other->GetType() == OBJECTTYPE::PLAYER ||
		(dynamic_pointer_cast<SkillObject>(_other) &&
			dynamic_pointer_cast<SkillObject>(_other)->GetOwner()->GetType() == OBJECTTYPE::PLAYER))
	{
		SetAttacked(true);
		cout << "몬스터가 공격받음" << endl;
	}
}

void Wolf::OnCollisionExit(shared_ptr<GameObject> _other)
{
	SetAttacked(false);
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
	m_model->ReadAnimation(L"Atk1", L"wolf/wolf_atk1_anim");
	m_model->ReadAnimation(L"Atk2", L"wolf/wolf_atk2_anim");
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
	m_animationStateMachine = make_shared<AnimationStateMachine>(AnimationStateType::Wait);
	AddComponent(m_animationStateMachine);
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait,			make_shared<WolfAnimWaitState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Appear,		make_shared<WolfAnimAppearState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Run,			make_shared<WolfAnimRunState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::BaseAttack,	make_shared<WolfAnimAttackState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Death,		make_shared<WolfAnimDeathState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Dying,		make_shared<WolfAnimDyingState>());
	GetAnimationStateMachine()->RegisterState(AnimationStateType::Trace,		make_shared<WolfAnimTraceState>());

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

	// Wolf 달리기
	vector<wstring> runAnims = { L"Run" };
	vector<float> runAnimsDurations;
	runAnimsDurations.push_back(animator->GetAnimationDuration(L"Run"));
	animator->CreateSequence(L"Wolf_Run_Sequence", runAnims, runAnimsDurations, true);


	// Wolf 공격모션 1
	vector<wstring> atk1Anims = { L"Atk1" };
	vector<float> atk1AnimsDurations;
	atk1AnimsDurations.push_back(animator->GetAnimationDuration(L"Atk1"));
	animator->CreateSequence(L"Wolf_Atk1_Sequence", atk1Anims, atk1AnimsDurations, false);

	// Wolf 공격모션 2
	vector<wstring> atk2Anims = { L"Atk2" };
	vector<float> atk2AnimsDurations;
	atk2AnimsDurations.push_back(animator->GetAnimationDuration(L"Atk2"));
	animator->CreateSequence(L"Wolf_Atk2_Sequence", atk2Anims, atk2AnimsDurations, false);

}

void Wolf::InitWolfComponent()
{
	m_collider = make_shared<SphereCollider>();
	m_collider->SetOffset(Vec3(0, 1, 0));
	m_collider->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	m_collider->SetVisible(true);

	//m_collider->SetVisible(false);
	m_rigidbody = make_shared<Rigidbody>();
	m_navAgent = make_shared<NavMeshAgent>();
	//m_itembox = make_shared<ItemBox>();

	////행동 스크립트? 컴포넌트?
	//auto attackScript = make_shared<WolfBaseAttack>();
	//attackScript->SetOwner(shared_from_this());
	//AddComponent(attackScript);

	//auto traceScript = make_shared<WolfTrace>();
	//traceScript->SetOwner(shared_from_this());
	//AddComponent(traceScript);

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
	m_monsterStateMachine = make_shared<MonsterStateMachine>();
	AddComponent(m_monsterStateMachine);


	auto self = static_pointer_cast<Monster>(shared_from_this());
	m_monsterInterface = make_shared<MonsterInterface>(self);

	m_monsterStateMachine->SetMonsterInterface(m_monsterInterface);

	m_monsterStateMachine->RegisterState(MonsterStateType::Wait, make_shared<WolfWaitState>());
	m_monsterStateMachine->RegisterState(MonsterStateType::Appear, make_shared<WolfAppearState>());
	m_monsterStateMachine->RegisterState(MonsterStateType::Run, make_shared<WolfRunState>());
	m_monsterStateMachine->RegisterState(MonsterStateType::Death, make_shared<WolfDeathState>(shared_from_this()));
	m_monsterStateMachine->RegisterState(MonsterStateType::Dying, make_shared<WolfDyingState>());
	m_monsterStateMachine->RegisterState(MonsterStateType::Trace, make_shared<WolfTraceState>(shared_from_this()));
	m_monsterStateMachine->RegisterState(MonsterStateType::Attack, make_shared<WolfAttackState>(shared_from_this()));
}

void Wolf::InitWolfStats()
{
	m_monsterStatus.maxHp = 200;
	m_monsterStatus.hp = 200;
}


//BT

void Wolf::InitBehaviorTree()
{
	// 1. BehaviorTree 인스턴스 생성
	m_behaviorTree = make_shared<BehaviorTree>();

	// 2. 루트 노드 (Selector) 생성
	auto rootNode = make_shared<SelectorNode>();

	// -----------------------------------------------------------
	// 3. 하위 시퀀스 조립 (Delegate::Bind 활용)
	// -----------------------------------------------------------

	// [1] 사망 시퀀스 (Sequence: CheckHP -> Die)
	auto deathSeq = make_shared<SequenceNode>();
	deathSeq->AddChild(make_shared<ActionNode>(Delegate::Bind(&Wolf::CheckHP, this)));
	deathSeq->AddChild(make_shared<ActionNode>(Delegate::Bind(&Wolf::Die, this)));

	// [2] 전투 시퀀스 (Sequence: CheckAttackRange -> Attack)
	auto attackSeq = make_shared<SequenceNode>();
	attackSeq->AddChild(make_shared<ActionNode>(Delegate::Bind(&Wolf::CheckAttackRange, this)));
	attackSeq->AddChild(make_shared<ActionNode>(Delegate::Bind(&Wolf::Attack, this)));

	// [3] 추적 시퀀스 (Sequence: CheckDetectRange -> Trace)
	auto traceSeq = make_shared<SequenceNode>();
	traceSeq->AddChild(make_shared<ActionNode>(Delegate::Bind(&Wolf::CheckDetectRange, this)));
	traceSeq->AddChild(make_shared<ActionNode>(Delegate::Bind(&Wolf::Trace, this)));

	// [4] 대기 액션 (Action)
	auto idleNode = make_shared<ActionNode>(Delegate::Bind(&Wolf::Idle, this));

	// 4. 루트에 자식 등록 (우선순위: 사망 > 공격 > 추적 > 대기)
	rootNode->AddChild(deathSeq);
	rootNode->AddChild(attackSeq);
	rootNode->AddChild(traceSeq);
	rootNode->AddChild(idleNode);

	// 5. 완성된 루트 노드를 BehaviorTree에 등록
	m_behaviorTree->SetRootNode(rootNode);
}

NodeState Wolf::CheckHP()
{
	// 체력이 0 이하면 사망 로직(Die) 실행
	if (m_monsterStatus.hp <= 0)
	{
		return NodeState::SUCCESS;
	}

	return NodeState::FAILURE;
}

NodeState Wolf::Die()
{
	auto animSM = GetAnimationStateMachine();

	// -----------------------------------------------------------
	// 1. 이미 사망 로직이 시작된 경우
	// -----------------------------------------------------------
	if (m_isDeadMotionStarted)
	{
		// 1-1. 이미 사체(Dying) 상태라면 -> 계속 유지 (완벽한 죽음)
		if (animSM->IsInState(AnimationStateType::Dying))
		{
			return NodeState::RUNNING;
		}
			
		animSM->RequestStateChange(AnimationStateType::Dying);
	
		return NodeState::RUNNING;
	}

	// -----------------------------------------------------------
	// 2. 최초 사망 진입
	// -----------------------------------------------------------

	// 이동 정지
	if (m_navAgent) {
		m_navAgent->Stop();
		m_navAgent->SetDestination(GetTransform()->GetPosition());
	}

	// 사망 시작
	cout << "BT: 늑대 사망 시작 (애니메이션 요청)" << endl;
	animSM->RequestStateChange(AnimationStateType::Death);

	// 변수 초기화
	m_isDeadMotionStarted = true;
	m_deathTimer = 0.0f; // 타이머 0으로 시작

	return NodeState::RUNNING;
}

NodeState Wolf::CheckAttackRange()
{
	auto target = GetTarget();
	if (!target || !target->GetActive()) return NodeState::FAILURE;

	float dist = Vec3::Distance(GetTransform()->GetPosition(), target->GetTransform()->GetPosition());


	// 사거리 체크 (InitWolfStats에서 hitRange가 충분히 큰지 확인 필요, 예: 2.5f)
	if (dist <= m_monsterStatus.hitRange)
	{
		if (m_navAgent) m_navAgent->Stop();
		return NodeState::SUCCESS;
	}
	m_isAttacking = false;
	return NodeState::FAILURE;
}

NodeState Wolf::Attack()
{
	auto target = GetTarget();
	if (!target) return NodeState::FAILURE;

	auto animSM = GetAnimationStateMachine();

	if (m_navAgent) m_navAgent->Stop();
	// --------------------------------------------------------
	// 1. 공격 시작 진입 (처음 들어왔을 때)
	// --------------------------------------------------------
	if (!m_isAttacking)
	{
		// [중요] 혹시라도 이전 상태가 남아있을 수 있으니 변수 확실히 초기화
		m_attackTimer = 0.0f;

		// 회전 로직...
		Vec3 dir = target->GetTransform()->GetPosition() - GetTransform()->GetPosition();
		dir.y = 0; dir.Normalize();
		float targetYaw = atan2(dir.x, dir.z) + XM_PI;
		GetTransform()->SetLocalRotation(Vec3(0, targetYaw * 180.0f / XM_PI, 0));

		// [중요] 애니메이션 상태 강제 전환
		// 현재 상태가 이미 BaseAttack이라도, 모션 처음부터 다시 틀어야 하므로
		// (만약 FSM이 같은 상태 전이를 무시한다면) 잠시 Wait를 거쳤다 가거나 해야 함.
		// 하지만 보통은 그냥 요청하면 됨.

		// 디버깅용 로그
		cout << "BT: 공격 애니메이션 요청 (Trace -> BaseAttack)" << endl;
		animSM->RequestStateChange(AnimationStateType::BaseAttack);

		// 공격 스크립트 시작
		auto attackScript = GetComponent<WolfBaseAttack>();
		if (attackScript) attackScript->StartAttack();

		m_isAttacking = true; // 플래그 설정
		return NodeState::RUNNING;
	}

	// --------------------------------------------------------
	// 2. 공격 진행 중 (Update 로직)
	// --------------------------------------------------------
	m_attackTimer += DT;

	
	// 공격 애니메이션 시간이 지났을 때 (한 사이클 완료)
	if (m_attackTimer >= m_attackDuration)
	{
		// 거리 체크 (기존 로직: 3.0f 이내면 연속 공격)
		float dist = Vec3::Distance(GetTransform()->GetPosition(), target->GetTransform()->GetPosition());

		if (dist <= 3.0f) // 사거리 내 유지 중
		{
			cout << "BT: 연속 공격 실행" << endl;
			m_attackTimer = 0.0f; // 타이머 리셋

			// [중요] 애니메이션 재시작 요청
			// AnimStateMachine이 같은 상태 요청을 무시할 수 있으므로 
			// 내부적으로 'Replay' 처리가 되어 있거나, 
			// 여기서 잠시 Wait로 갔다가 다시 BaseAttack을 해야 할 수 있음.
			// 하지만 기존 코드(WolfAnimAttackState::Enter)에 모션 토글이 있으므로 
			// Enter가 다시 불리게만 하면 됨.

			// 강제로 다시 Enter를 타게 하기 위해 잠시 Wait 요청 후 바로 Attack 요청은 불가능(한 프레임 내).
			// WolfAnimAttackState가 '같은 상태 전이'를 허용하거나,
			// AnimStateMachine::ExecuteStateChange를 강제로 부를 수 있어야 함.

			// 가장 깔끔한 방법: AnimStateMachine에 'Replay' 기능이 없다면,
			// ForceRestart 같은 함수를 만들거나, 
			// 단순히 RequestStateChange를 다시 호출했을 때 Enter가 불리는지 확인 필요.
			// (보통 FSM은 같은 상태면 무시함)

			// 기존 WolfAttackState에서도 RequestStateChange(BaseAttack)을 불렀으니
			// 아마 AnimStateMachine이나 State가 이를 처리해주는 것으로 보임.
			animSM->RequestStateChange(AnimationStateType::BaseAttack);

			// WolfBaseAttack 컴포넌트 리셋
			auto attackScript = GetComponent<WolfBaseAttack>();
			if (attackScript) attackScript->StartAttack(); // 타이머 리셋 등
		}
		else
		{
			// 사거리 벗어남->공격 종료
			cout << "BT: 공격 종료 (사거리 벗어남)" << endl;
			m_isAttacking = false;
			m_attackTimer = 0.0f;

			auto attackScript = GetComponent<WolfBaseAttack>();
			if (attackScript) attackScript->StopAttack();

			// [수정] 여기서 Wait로 바꾸지 말고 그냥 SUCCESS 리턴!
			// 다음 프레임에 Trace()가 호출되면서 자연스럽게 Run으로 바꿉니다.
			// animSM->RequestStateChange(AnimationStateType::Wait); // <-- 이 줄 삭제 또는 주석 처리

			return NodeState::SUCCESS;
		}
	}

	// 아직 공격 모션 진행 중
	return NodeState::RUNNING;
}

NodeState Wolf::CheckDetectRange()
{
	auto target = GetTarget();
	if (target)
	{
		// 30미터까지는 무조건 추적
		if (Vec3::Distance(GetTransform()->GetPosition(), target->GetTransform()->GetPosition()) <= 30.f)
			return NodeState::SUCCESS;
	}
	return NodeState::FAILURE;
}

NodeState Wolf::Trace()
{
	auto target = GetTarget();
	if (!target) return NodeState::FAILURE;

	// 1. 이동 처리 (NavMeshAgent)
	if (m_navAgent)
	{
		m_navAgent->SetDestination(target->GetTransform()->GetPosition());
		m_navAgent->SetSpeed(2.0f); // 추적 속도
	}

	// 2. 애니메이션 처리
	auto animSM = GetAnimationStateMachine();
	if (!animSM->IsInState(AnimationStateType::Trace))
	{
		animSM->RequestStateChange(AnimationStateType::Trace);
	}

	// 추적은 '완료'라는 개념이 없음 (타겟을 잡을 때까지 계속함)
	// 따라서 항상 RUNNING 반환
	return NodeState::RUNNING;
}

NodeState Wolf::Idle()
{
	cout << "Idle 진입\n";

	// 확실하게 멈춤 명령
	if (m_navAgent) {
		m_navAgent->Stop();
		m_navAgent->SetDestination(GetTransform()->GetPosition()); // 현재 위치로 강제 설정
	}

	// 타겟 해제는 신중하게 (여기서 해제하면 추적이 끊길 수 있음)
	// SetTarget(nullptr); // <-- 이 줄 때문에 추적이 끊길 수 있으니 주석 처리 해보세요!

	auto animSM = GetAnimationStateMachine();
	if (!animSM->IsInState(AnimationStateType::Wait))
	{
		animSM->RequestStateChange(AnimationStateType::Wait);
	}
	return NodeState::SUCCESS;
}
