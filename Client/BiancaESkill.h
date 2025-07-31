#pragma once
#include "BaseSkill.h"
class BiancaESkill :
    public BaseSkill
{
    using Super = BaseSkill;
public:
    BiancaESkill(shared_ptr<Player> _player);
    virtual ~BiancaESkill();

public:
    virtual void PlaySkill() override;
    virtual void Update() override;
};

