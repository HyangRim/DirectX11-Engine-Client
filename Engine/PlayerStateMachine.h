#pragma once
#include "Component.h"

enum class PlayerStateType
{
    Wait,
    Move,
    Attack,
    Die,

};

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

    PlayerStateType GetType() const { return m_type; }

protected:
    PlayerStateType m_type;
};


class PlayerStateMachine :
    public Component
{
public:
    PlayerStateMachine();
    ~PlayerStateMachine();

    // Component 주요 함수 오버라이드
    virtual void Init() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnDestroy() override;

    // 상태 관리
    void ChangeState(PlayerStateType newState);
    bool CanChangeState(PlayerStateType newState);


    // 상태 등록 함수 (외부에서 상태 등록 가능)
    void RegisterState(PlayerStateType type, shared_ptr<PlayerState> state);

    PlayerState GetCurrentState() const;

private:
    unordered_map<PlayerStateType, shared_ptr<PlayerState>> m_states;
    shared_ptr<PlayerState> m_currentState;
};

