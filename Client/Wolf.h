#pragma once
#include "Monster.h"

#include "BehaviorTree.h"

class Wolf :
    public Monster
{
    using Super = Monster;
public:
    Wolf(shared_ptr<Shader> _shader);
    virtual ~Wolf();

public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void LateUpdate() override;
    virtual void FixedUpdate() override;

    //Collision 관련
    virtual void OnCollision(shared_ptr<GameObject> _other) override;
    virtual void OnCollisionEnter(shared_ptr<GameObject> _other) override;
    virtual void OnCollisionExit(shared_ptr<GameObject> _other) override;

private:
    void UpdateState();

private:
    void InitWolfModel();
    void InitWolfAnimation();
    void InitWolfComponent();
    void InitWolfAI();
    void InitWolfMSM();
    void InitWolfStats();



    //BT
private:
    // 1. BehaviorTree 인스턴스 선언
    shared_ptr<BehaviorTree> m_behaviorTree;

    // 2. 트리 조립 함수 (Start에서 호출)
    void InitBehaviorTree();

    // 3. ActionNode에 연결될 실제 멤버 함수들 (NodeState 반환)
    NodeState CheckHP();
    NodeState Die();
    NodeState CheckAttackRange();
    NodeState Attack();
    NodeState CheckDetectRange();
    NodeState Trace();
    NodeState Idle();

private:
    bool m_damageDealt = false; // 이번 공격 애니메이션에서 데미지를 줬는지 체크

    float m_attackTimer = 0.0f;       // 공격 쿨타임/딜레이 체크용 (기존 m_animTime)
    float m_attackDuration = 36.f / 25.f; // 공격 애니메이션 길이 (기존 로직 값)
    bool m_isAttacking = false;       // 현재 공격 시퀀스 진행 중인지 여부

    bool m_isDeadMotionStarted = false; // 사망 로직이 시작되었는지 체크
    float m_deathTimer = 0.0f; // 사망 애니메이션 진행 시간 체크용

    // 매 프레임 경로를 다시 구하지 않게 (마리수만큼 FindPath 가 쌓이는 것 방지)
    bool m_idleEntered = false;       // Idle 에 이미 들어와서 멈춤 명령을 보냈는지
    bool m_idleThisFrame = false;     // 이번 프레임 행동 트리가 Idle 을 실행했는지
    bool m_hasRepathTarget = false;   // 추적 경로를 한 번이라도 구했는지
    Vec3 m_lastRepathTarget;          // 마지막으로 경로를 구한 타겟 위치
    float m_repathTimer = 0.f;        // 다음 경로 재계산까지 남은 시간
};

