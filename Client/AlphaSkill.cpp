#include "pch.h"
#include "Alpha.h"
#include "AlphaSkill.h"
#include "Player.h"
#include "BiancaESkillCircle.h"

AlphaSkill::AlphaSkill(shared_ptr<Alpha> _alpha) : m_alpha(_alpha)
{
}

AlphaSkill::~AlphaSkill()
{
}

void AlphaSkill::Start()
{
	for (int idx = 0; idx < 5; ++idx) {
		m_circleObjects[idx] = make_shared<BiancaESkillCircle>();

		m_circleObjects[idx]->GetTransform()->SetScale(Vec3(1.f, 0.03f, 1.f));
		m_circleObjects[idx]->AddComponent(make_shared<SphereCollider>());
		m_circleObjects[idx]->GetCollider()->SetOffsetScale(Vec3(1.f, 30.f, 1.f));
		m_circleObjects[idx]->SetActive(false);
	}
}

void AlphaSkill::Play()
{
	m_isActive = true;
	SOUND->PlaySound(L"Alpha/AlphaOmega_skill02_Ready.wav", 22, 0.5f);

	//m_circleObject들 여러 개 배치. 
	Vec3 alphaPosition = m_alpha->GetTransform()->GetPosition();
	Vec3 alphalook = m_alpha->GetTransform()->GetLook();
	for (int idx = 0; idx < 5; ++idx) {
		m_circleObjects[idx]->DamageFlag(true);

		float randomAngle = (rand() % 1200 - 600) * 0.1f;
		float angleRadians = randomAngle * (3.14159f / 180.f);
		
		float randomDistance = (rand() % 650) * 0.01f;

		Vec3 direction;
		direction.x = alphalook.x * cos(angleRadians) - alphalook.z * sin(angleRadians);
		direction.z = alphalook.x * sin(angleRadians) + alphalook.z * cos(angleRadians);
		direction.y = alphaPosition.y;

		Vec3 finalPos = alphaPosition + direction * randomDistance;

		m_circleObjects[idx]->GetTransform()->SetPosition(finalPos);
		m_circleObjects[idx]->SetActive(true);
	}
}

void AlphaSkill::Update()
{
	if (m_isActive && m_skillElapsedTime <= m_skillDuration) {
		m_skillElapsedTime += DT;
	}
	else if(m_isActive && m_skillElapsedTime > m_skillDuration){
		if (!m_boozer) {
			SOUND->PlaySound(L"Alpha/AlphaOmega_skill02_Spout.wav", 22, 0.5f);
			unordered_set<shared_ptr<GameObject>> allobjects;
			for (int idx = 0; idx < 5; ++idx) {
				m_circleObjects[idx]->DamageFlag(false);
				auto objects = m_circleObjects[idx]->GetCollisionObjects();

				for (auto& object : objects) {
					allobjects.insert(object);
				}
			}
			m_boozer = true;

			//여기서 allobject보내서 데미지 주기. 

		}
		m_skillEffectElapsedTime += DT;
	}
	else if (m_isActive && (m_skillElapsedTime > m_skillDuration) && (m_skillEffectElapsedTime > m_skillEffectDuration)) {
		m_boozer = false;
		m_isActive = false;
		m_skillEffectElapsedTime = 0.f;
		m_skillElapsedTime = 0.f;

		for (int idx = 0; idx < 5; ++idx) {
			m_circleObjects[idx]->SetActive(false);
		}
	}
}
