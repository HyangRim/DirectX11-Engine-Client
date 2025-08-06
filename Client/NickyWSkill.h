#pragma once
#include "BaseSkill.h"
class NickyWSkill :
    public BaseSkill
{
    using Super = BaseSkill;

public:
    NickyWSkill(shared_ptr<Player> _player);
    virtual ~NickyWSkill();

public:
    virtual void PlaySkill() override;
    virtual void Update() override;

private:
    shared_ptr<Shader> m_shader = nullptr;
};

