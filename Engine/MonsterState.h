#pragma once

enum class MonsterStateType
{
    Wait,
    Appear,
    Move,
    Death,
    Dying
};

class MonsterState
{
public:
    MonsterState(MonsterStateType type) : m_type(type) {}
    virtual ~MonsterState() = default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;
    virtual bool CanTransitionTo(MonsterStateType newState) = 0;

    MonsterStateType GetType() const { return m_type; }

protected:
    MonsterStateType m_type;
    float m_expectedDuration;
};