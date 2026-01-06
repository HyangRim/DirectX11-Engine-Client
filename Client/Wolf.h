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
};

