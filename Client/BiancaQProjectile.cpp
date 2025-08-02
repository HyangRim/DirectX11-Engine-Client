#include "pch.h"
#include "BiancaQProjectile.h"
#include "Player.h"
#include "Monster.h"

BiancaQProjectile::BiancaQProjectile()
{
}

BiancaQProjectile::~BiancaQProjectile()
{
}

void BiancaQProjectile::Start()
{
}

void BiancaQProjectile::Update()
{
	if (!m_moving || !GetActive()) return;

	//BiancaQSkill에서 SetMoveTarget로 설정해주면 바로 움직임. 
	Vec3 curPos = GetTransform()->GetPosition();

	if (Vec3::Distance(curPos, m_endPos) <= 0.01f) {
		//도착하면 꺼지고 Cone생성. (생성은 BiancaQSkill에서)
		SOUND->PlaySound(L"Bianca_Skill01_Active.wav", 1, 0.5f);
		m_arrive = true;
		SetActive(false);
		m_moving = false;
	}
	// 위치 업데이트
	Vec3 newPos = curPos + m_direction * m_speed * DT;
	GetTransform()->SetPosition(newPos);
}

void BiancaQProjectile::OnCollision(shared_ptr<GameObject> _other)
{
}

void BiancaQProjectile::OnCollisionEnter(shared_ptr<GameObject> _other)
{
	auto player = dynamic_pointer_cast<Player>(_other);
	auto monster = dynamic_pointer_cast<Monster>(_other);

	if (player != nullptr) {
		SOUND->PlaySound(L"Bianca_Skill01_Hit02.wav", 1, 0.5f);
	}
	if (monster != nullptr) {
		SOUND->PlaySound(L"Bianca_Skill01_Hit01.wav", 1, 0.5f);
	}
}

void BiancaQProjectile::OnCollisionExit(shared_ptr<GameObject> _other)
{
}

void BiancaQProjectile::SetMoveTarget(Vec3& _startPos, Vec3& _endPos)
{
	m_startPos = _startPos;
	m_endPos = _endPos;
	m_direction = m_endPos - m_startPos;
	m_direction.Normalize();
	m_moving = true;
}
