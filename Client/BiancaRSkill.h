#pragma once
#include "BaseSkill.h"
class BiancaRSkill :
    public BaseSkill
{
    using Super = BaseSkill;
public:
    BiancaRSkill(shared_ptr<Player> _player);
    virtual ~BiancaRSkill();

public:
    virtual void PlaySkill() override;
    virtual void Update() override;
};

