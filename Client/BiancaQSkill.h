#pragma once
#include "BaseSkill.h"
class BiancaQSkill :
    public BaseSkill
{
    using Super = BaseSkill;
public:
    BiancaQSkill(shared_ptr<Player> _player);
    virtual ~BiancaQSkill();

public:
    virtual void PlaySkill() override;
    virtual void Update() override;
};

