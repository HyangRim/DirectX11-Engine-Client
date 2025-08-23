#include "pch.h"
#include "BiancaESkillCircle.h"

BiancaESkillCircle::BiancaESkillCircle()
{
}

BiancaESkillCircle::~BiancaESkillCircle()
{
}

void BiancaESkillCircle::Start()
{
	Super::Start();
}

void BiancaESkillCircle::Update()
{
	Super::Update();
	m_object.clear();
}

void BiancaESkillCircle::LateUpdate()
{
	Super::LateUpdate();
}

void BiancaESkillCircle::OnCollision(shared_ptr<GameObject> _other)
{
	if(m_damageFlag)
		m_object.insert(_other);
}
