#pragma once
#include "BaseSkill.h"
class BiancaQProjectile;
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


    float m_skillRange = 5.f;
    shared_ptr<BiancaQProjectile> m_Projectile;
};

