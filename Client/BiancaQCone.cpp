#include "pch.h"
#include "BiancaQCone.h"
#include "Player.h"
#include "Monster.h"

BiancaQCone::BiancaQCone()
{
}

BiancaQCone::~BiancaQCone()
{
}

void BiancaQCone::Start()
{
}

void BiancaQCone::Update()
{
	if (!GetActive()) return;

	//m_lifeTime동안만 살아남을 수 있음. 
	m_timer += DT;
	if (m_timer > m_lifeTime) {
		SetActive(false);
		m_isBind = false;
		m_upElapsedTime = 0.f;
		m_targetMonster.reset();
		m_targetPlayer.reset();
	}

	if (m_targetPlayer != nullptr || m_targetMonster != nullptr) {
		//CONE 솟아오르기. 
		if (m_upDuration <= m_upElapsedTime)
			return;

		m_upElapsedTime += DT;

		float t = m_upElapsedTime / m_upDuration;
		float yLerp = Utils::FLerp(m_startY, m_endY, t);

		Vec3 curPos = GetTransform()->GetPosition();
		curPos.y = yLerp;
		GetTransform()->SetPosition(curPos);
	}
}

void BiancaQCone::OnCollisionEnter(shared_ptr<GameObject> _other)
{
	m_targetPlayer = dynamic_pointer_cast<Player>(_other);
	m_targetMonster = dynamic_pointer_cast<Monster>(_other);

	if (m_targetPlayer != nullptr || m_targetMonster != nullptr) {
		m_isBind = true;
		m_startY = GetTransform()->GetPosition().y;
		m_endY = m_startY + 5.f;

		//그 대상을 CASTING(BIND) 상태로 바꿈.


		//데미지 줌. 

		SOUND->PlaySound(L"Bianca_Skill01_End.wav", 1, 0.5f);
	}
}
