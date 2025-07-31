#pragma once
#include "BaseSkill.h"

class Player;

class NickyESkill :
    public BaseSkill
{
    using Super = BaseSkill;

public:
    NickyESkill(shared_ptr<Player> _player);
    virtual ~NickyESkill();

public:
    virtual void PlaySkill() override;
    virtual void Update() override;

private:
    shared_ptr<Shader> m_shader = nullptr;
};

