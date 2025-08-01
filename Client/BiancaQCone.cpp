#include "pch.h"
#include "BiancaQCone.h"

BiancaQCone::BiancaQCone(float _timer)
	: m_timer(_timer)
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
	m_timer += DT;
	if (m_timer > m_lifeTime) {
		SetActive(false);
	}
}

void BiancaQCone::OnCollisionEnter(shared_ptr<GameObject> _other)
{
}
