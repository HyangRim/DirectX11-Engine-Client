#pragma once
#include "BaseSkill.h"
class BiancaQProjectile;
class BiancaQCone;
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
    unique_ptr<BiancaQProjectile> m_Projectile;
    unique_ptr<BiancaQCone> m_Cone;
};

