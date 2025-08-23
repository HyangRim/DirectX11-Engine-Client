#pragma once
#include "Component.h"

#include "Delegate.h"

enum class PlayerStateType
{
    Wait,
    Run,
    Skill_1,
    Skill_2,
    Skill_3,
    Skill_4,
    Craft,
    Die,
    BaseAttack,
    Counter,
};
class IPlayer;

// 상태 인터페이스 (State Pattern 기본틀)
class PlayerState
{
public:
    PlayerState(PlayerStateType type) : m_type(type) {}
    virtual ~PlayerState() = default;

    virtual void Enter() = 0;             // 상태 시작 시 호출
    virtual void Update() = 0;   // 매 프레임 호출
    virtual void Exit() = 0;              // 상태 종료 시 호출
    virtual bool CanTransitionTo(PlayerStateType newState) = 0;

    // 새로 추가할 가상 함수들
    virtual bool IsCharging() const { return false; }      // 차징 중인지 확인
    virtual bool IsReleasing() const { return false; }     // 릴리즈 중인지 확인
    virtual bool IsMovable() const { return true; }        // 이동 가능한지 확인

    PlayerStateType GetType() const { return m_type; }

    void SetRecipeIndex(int index) { m_recipeIndex = index; }
    
    void SetTarget(shared_ptr<GameObject> _target) { m_target = _target; }
    shared_ptr<GameObject> GetTarget() { return m_target; }

protected:
    PlayerStateType m_type;
    shared_ptr<GameObject> m_target;
    int m_recipeIndex = 0;
};


class PlayerStateMachine :
    public Component
{
public:
    PlayerStateMachine(shared_ptr<AnimationStateMachine> animationStateMachine, int chargingInfo, int isMovableOnSkill, int isNeedTarget);
    ~PlayerStateMachine();

    // Component 주요 함수 오버라이드
    virtual void Init() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnDestroy() override;

    // 상태 관리
    void ChangeState(PlayerStateType newState);
    bool CanChangeState(PlayerStateType newState);

    void ProcessInput();
    //void ProcessAnimationFSM();
    Ray CreateRayFromMouse(POINT mousePos, shared_ptr<Camera> camera);

    bool IsInState(PlayerStateType state) const;

    // 상태 등록 함수 (외부에서 상태 등록 가능)
    void RegisterState(PlayerStateType type, shared_ptr<PlayerState> state);

    PlayerStateType GetCurrentState() const; //타입만 넘겨줌
    shared_ptr<PlayerState> GetCurState() { return m_currentState; } //state 포인터를 넘겨줌
    shared_ptr<PlayerState> GetState(PlayerStateType type) { return m_states[type]; }

    void HandleSpecialStateTransitions();

    void SetPlayerInterface(shared_ptr<IPlayer> pIayerInterface) { m_playerInterface = pIayerInterface; }

    void PrintCurState();

private:
    shared_ptr<ModelAnimator> m_modelAnimator;
    unordered_map<PlayerStateType, shared_ptr<PlayerState>> m_states;
    shared_ptr<PlayerState> m_currentState;

    shared_ptr<AnimationStateMachine> m_animationStateMachine;

    int m_chargingInfo;
    int m_isMovableOnSkill; //스킬 시전 중 우클릭으로 움직일 수 없는 스킬 Q,W,E,R -> 8, 4, 2, 1
    int m_isNeedTarget; //타겟이 필요한 스킬 목록 Q,W,E,R -> 8, 4, 2, 1


private:
    shared_ptr<GameObject> m_attackTarget; // 공격 대상 저장
    bool m_isMovingToAttack = false; // 공격을 위해 이동 중인지
    float m_baseAttackDelay = (38.f / 25.f) / 2.f;
    float m_baseAttackDelayDuration = 0.f;

public:
    void SetAttackTarget(shared_ptr<GameObject> target);
    void MoveToAttackTarget(Vec3 targetPos, float attackRange);
    void StartBaseAttack();


private:
    shared_ptr<IPlayer> m_playerInterface;


private:
    bool CheckTargetForSkill(KEY_TYPE skillKey);
    shared_ptr<GameObject> GetPickedTargetAtMouse();

    bool IsSkillOnCooldown(int skillIndex);

public:
    /*using SkillUsedDelegate = Delegate::Delegate<int>;

    SkillUsedDelegate OnSkillUsed;*/

public:
   
    Delegate::Delegate<bool&> OnTryCraftFirst;

    // 델리게이트에 타겟 정보 추가
    using SkillUsedDelegate = Delegate::Delegate<int, shared_ptr<GameObject>>;
    using SkillCooldownCheckDelegate = Delegate::Delegate<int, bool&>; // (skillIndex, OUT isOnCooldown)
    SkillUsedDelegate OnSkillUsed;
    SkillCooldownCheckDelegate OnSkillCooldownCheck;



private:
    void HandleStateChangeRequest(shared_ptr<EventData> eventData);
    void ChangeStateImmediate(PlayerStateType newState);
};

