#pragma once
#include "GameObject.h"
class BiancaQProjectile : public GameObject
{
public:
	BiancaQProjectile();
	virtual ~BiancaQProjectile();
	
public:
    virtual void Start() override;
    virtual void Update() override;

    //Collision ฐüทร
    virtual void OnCollision(shared_ptr<GameObject> _other) override;
    virtual void OnCollisionEnter(shared_ptr<GameObject> _other) override;
    virtual void OnCollisionExit(shared_ptr<GameObject> _other) override;

public:
    void SetMoveTarget(Vec3& _startPos, Vec3& _endPos);

private:
    bool m_moving = false;
    float m_speed = 10.f;
    Vec3 m_startPos, m_endPos;
};

