#pragma once
#include "Component.h";
#include "MonsterState.h"

class MonsterStateMachine : public Component
{
public:
    MonsterStateMachine(shared_ptr<AnimationStateMachine> animationStateMachine);
    ~MonsterStateMachine();

    virtual void Start() override;
    virtual void Update() override;

    void ChangeState(MonsterStateType newState);
    bool CanChangeState(MonsterStateType newState);
    MonsterStateType GetCurrentState() const;

    void RegisterState(MonsterStateType type, shared_ptr<MonsterState> state);

    void ProcessInput();

    void HandleSpecialStateTransitions();

private:
    unordered_map<MonsterStateType, shared_ptr<MonsterState>> m_states;
    shared_ptr<MonsterState> m_currentState;
    shared_ptr<AnimationStateMachine> m_animationStateMachine;
};

