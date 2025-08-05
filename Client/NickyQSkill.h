#pragma once
#include "BaseSkill.h"
class NickyQSkill :
    public BaseSkill
{
    using Super = BaseSkill;

public:
    NickyQSkill(shared_ptr<Player> _player);
    ~NickyQSkill();


public:
    virtual void PlaySkill() override;
    virtual void Update() override;

    void CalculateSkillDirection();
    bool IsFirstAnimationPlaying();
private:
    shared_ptr<Shader> m_shader = nullptr;
   
    Vec3 m_startPos, m_targetPos;

    float m_maxRange = 5.f;
    bool m_bskillStart = false;
    float m_duration = 0.f;
    float m_speed = 15.f;
    bool m_moveFlag = false;

    float m_moveDuration = 0.f;
    float m_moveElapsedTime = 0.f;

    int soundCount = 2;
};

