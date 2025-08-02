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
    bool GetArrive() { return m_arrive; }
    void SetArrive(bool _arrive) { m_arrive = _arrive; }

private:
    bool m_moving = false;
    bool m_arrive = false;
    float m_speed = 10.f;
    Vec3 m_startPos, m_endPos;
    Vec3 m_direction;
};

