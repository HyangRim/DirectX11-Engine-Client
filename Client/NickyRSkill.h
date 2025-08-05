#pragma once
#include "BaseSkill.h"

class Player;

class NickyRSkill :
    public BaseSkill
{
    using Super = BaseSkill;

public:
    NickyRSkill(shared_ptr<Player> _player);
    ~NickyRSkill();

public:
    virtual void PlaySkill() override;
    virtual void Update() override;

    void SetTarget(shared_ptr<GameObject> _target) { m_target = _target; }

    void CalculateSkillDirection();
    bool IsRushAnimationPlaying();
    void SetRushDuration(float duration);

private:
    shared_ptr<Shader> m_shader = nullptr;
    shared_ptr<GameObject> m_target = nullptr;
    Vec3 m_startPos, m_targetPos;

    float m_maxRange = 5.f;
    bool m_bskillStart = false;
    float m_duration = 0.f;
    float m_speed = 15.f;
    bool m_moveFlag = false;

    float m_moveDuration = 0.f;
    float m_moveElapsedTime = 0.f;

    float m_rushSoundDuration = 0.f;

    int soundCount = 2;
};

