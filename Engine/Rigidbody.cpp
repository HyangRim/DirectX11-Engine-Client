#include "pch.h"
#include "Rigidbody.h"

Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
	, m_mass(1.f), m_fricCoeff(300.f), m_maxVelocity(Vec3(5.f, 5.f, 5.f))
{
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::FixedUpdate()
{
	float scala = m_Force.Length(); //���� ũ��. 

	if (scala <= 0.001f)
		return;

	//���� ����ȭ ���ְ�. 
	m_Force.Normalize();

	float accel = scala / m_mass; //�������� ���ӵ�.
	m_Accel = m_Force * accel;	//���� * ��Į�� / ����

	//m_Accel += m_vAccel;
	m_velocity += m_Accel * DT;

	//���ӵ� ���� �ڵ�. 
	Vec3 fricDir = -m_velocity;
	fricDir.Normalize();

	//������� ������ �ݴ� ���� ����. 
	Vec3 friction = fricDir * m_fricCoeff * DT;

	if (m_velocity.Length() <= friction.Length()) {
		m_velocity = Vec3(0.f, 0.f, 0.f);
	}
	else {
		m_velocity += friction;
	}

	//�ӵ� ���� �˻�. 
	//�� ���� ���� �˻��ؾ���.

	Move();

	//�� �ʱ�ȭ
	m_Force = Vec3(0, 0, 0);
	m_Accel = Vec3(0, 0, 0);
}

void Rigidbody::Move()
{
	float speed = m_velocity.Length();

	if (speed >= 0.001f) {
		Vec3 pos = GetTransform()->GetPosition();
		pos += m_velocity * DT;

		GetTransform()->SetPosition(pos);
	}
}
