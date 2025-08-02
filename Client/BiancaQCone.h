#pragma once
#include "GameObject.h"

class Player;
class Monster;
class BiancaQCone :
    public GameObject
{
public:
    BiancaQCone();
    virtual ~BiancaQCone();

public:
    virtual void Start() override;
    virtual void Update() override;

    virtual void OnCollisionEnter(shared_ptr<GameObject> _other) override;

private:
    bool m_isBind = false;
    float m_timer = 0.f;
    float m_lifeTime = 1.25f;

    shared_ptr<Player> m_targetPlayer;
    shared_ptr<Monster> m_targetMonster;


private:
    //움직임 전용 변수들
    float m_startY, m_endY;
    float m_upElapsedTime = 0.f;
    float m_upDuration = 0.5f;
};

