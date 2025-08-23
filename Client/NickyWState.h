#pragma once
#include "PlayerStateMachine.h"
class NickyWState :
    public PlayerState
{
    using Super = PlayerState;

public:
    NickyWState(shared_ptr<ModelAnimator> modelAnimator, shared_ptr<GameObject> _player);
    ~NickyWState();

    virtual void Enter();
    virtual void Update();
    virtual void Exit();
    virtual bool CanTransitionTo(PlayerStateType newState);

private:
    float m_skillTime = 0.0f;  // 대기 상태 지속 시간
    bool m_isAnimationStarted = false;
    bool m_isSkillComplete = false;  // 추가: 스킬 완료 플래그

    shared_ptr<GameObject> m_player;
    shared_ptr<ModelAnimator> m_modelAnimator;

    friend class PlayerStateMachine;
};

