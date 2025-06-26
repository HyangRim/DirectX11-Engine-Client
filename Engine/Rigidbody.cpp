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
	float scala = m_Force.Length(); //힘의 크기. 

	if (scala <= 0.001f)
		return;

	//방향 정규화 해주고. 
	m_Force.Normalize();

	float accel = scala / m_mass; //최종적인 가속도.
	m_Accel = m_Force * accel;	//방향 * 스칼라 / 질량

	//m_Accel += m_vAccel;
	m_velocity += m_Accel * DT;

	//가속도 관련 코드. 
	Vec3 fricDir = -m_velocity;
	fricDir.Normalize();

	//마찰계수 포함한 반대 방향 벡터. 
	Vec3 friction = fricDir * m_fricCoeff * DT;

	if (m_velocity.Length() <= friction.Length()) {
		m_velocity = Vec3(0.f, 0.f, 0.f);
	}
	else {
		m_velocity += friction;
	}

	//속도 제한 검사. 
	//축 별로 따로 검사해야함.

	Move();

	//힘 초기화
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
