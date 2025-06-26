#pragma once
#include "Component.h"
class Rigidbody :
    public Component
{
    using Super = Component;
public:
    Rigidbody();
    virtual ~Rigidbody();

public:
    virtual void FixedUpdate() override;


public:
    void AddForce(Vec3 _force) { m_Force += _force; }
    void SetMass(float _mass) { m_mass = _mass; }
    float GetMass() { return m_mass; }
    float GetSpeed() { return m_velocity.Length(); }

    void SetVelocity(Vec3 _velocity) { m_velocity = _velocity; }
    Vec3 GetVelocity() { return m_velocity; }
    void AddVelocity(Vec3 _velocity) { m_velocity += _velocity; }

    void SetMaxVelocity(Vec3 _maxVelocity) { m_maxVelocity = _maxVelocity; }

private:
    void Move();

private:

    Vec3 m_Force;
    Vec3 m_Accel;           //���ӵ� 
    Vec3 m_vAccel;          //���� ���ӵ�. 
    Vec3 m_velocity;        //�ӵ�
    Vec3 m_maxVelocity;     //�ִ� �ӷ�

    float m_mass;       //��ü ����
    float m_fricCoeff;  //���� ���

};

