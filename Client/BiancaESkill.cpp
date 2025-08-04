#include "pch.h"
#include "BiancaESkill.h"
#include "BiancaESkillCircle.h"
#include "Player.h"
#include "Utils.h"

BiancaESkill::BiancaESkill(shared_ptr<Player> _player)
	: Super(_player)
{
	{
		m_skillImage = RESOURCES->GetOrAddTexture(L"BiancaE", L"..\\Resources\\Textures\\UI\\SkillIcon\\SkillIcon_1042400.png");
	}

	{
		m_skillCooldown = 8.f;
		m_skillName = L"순환.";
		m_skillDesc = L"비앙카가 혈액의 고리를 충전하고 전방으로 돌진하여 스킬 피해를 입힙니다.";
		m_skillLevel = 1;
		m_skillImage = RESOURCES->GetOrAddTexture(L"BiancaE", L"..\\Resources\\Textures\\UI\\SkillIcon\\SkillIcon_1042400.png");
	}
	//Coffin 모델 생성. 
	{
		m_circle = make_shared<BiancaESkillCircle>();
		m_circle->AddComponent(make_shared<MeshRenderer>());

		m_circle->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"default"));
		m_circle->GetMeshRenderer()->SetMesh(RESOURCES->Get<Mesh>(L"Sphere"));
		m_circle->GetMeshRenderer()->GetMaterial()->SetCastShadow(false);
		m_circle->GetTransform()->SetLocalScale(Vec3(1.f, 0.03f, 1.f));

		m_collider = make_shared<SphereCollider>();
		m_circle->AddComponent(m_collider);
		m_collider->SetOffsetScale(Vec3(1.f, 30.f, 1.f));
		m_circle->SetActive(true);
		m_circle->GetTransform()->SetParent(m_playerObject->GetTransform());
		m_circle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.03f, 0.f));
		CURSCENE->Add(m_circle);
	}


}

BiancaESkill::~BiancaESkill()
{
}

void BiancaESkill::PlaySkill()
{

}

void BiancaESkill::Update()
{
	m_skillcurCooldown -= DT;
	if (m_skillcurCooldown > 0.f)
		return;

	if (INPUT->GetButtonDown(KEY_TYPE::E)) {
		//17 eID사용. 
		SOUND->PlaySound(L"Bianca/Bianca_Skill03_Charge.wav", 17, 0.5f);
	}

	if (INPUT->GetButton(KEY_TYPE::E)) {
		m_circle->SetActive(true);
		m_circleSizeElapedTime = m_circleKeepElapedTime += DT;

		float circleSize = 1.f + (m_circleSizeElapedTime / m_circleSizeDuration) * 2.f;
		if (circleSize >= 3.f) circleSize = 3.f;
		Vec3 scale = Vec3(1.f * circleSize, 0.05f, 1.f * circleSize);

		//cout << "startPos : " << scale.x << " " << scale.y << " " << scale.z << "\n";
		
		m_circle->GetTransform()->SetScale(scale);
	}
	else if(INPUT->GetButtonUp(KEY_TYPE::E) || m_circleKeepElapedTime > 4.f){
		//E키 떼었을 때. 
		//얼마나 나갈지 세팅.
		float range =  m_maxRange * min(1.f, m_circleKeepElapedTime / m_circleSizeDuration);
		POINT mousePos = INPUT->GetMousePos();

		XMVECTOR mouseWorldPos = ScreenToWorld(mousePos);

		XMVECTOR playerPos = m_playerObject->GetTransform()->GetPosition();
		XMVECTOR direction = XMVector3Normalize(mouseWorldPos - playerPos);
		XMVECTOR skillTargetPos = XMVectorAdd(playerPos, XMVectorScale(direction, range));
		m_startPos = m_playerObject->GetTransform()->GetPosition();
		m_targetPos = skillTargetPos;

		//cout << "startPos : " << m_startPos.x << " " << m_startPos.y << " " << m_startPos.z << "\n";
		//cout << "m_targetPos : " << m_targetPos.x << " " << m_targetPos.y << " " << m_targetPos.z << "\n";

		float distance = Vec3::Distance(m_startPos, m_targetPos);
		m_moveDuration = distance / m_speed;
		m_moveElapsedTime = 0.f;
		// 회전 계산 및 적용
		float targetYaw = atan2(XMVectorGetX(direction), XMVectorGetZ(direction)) + 3.141592f; //3.141592 더해야 방향 제대로 됨
		Vec3 currentRotation = m_playerObject->GetTransform()->GetLocalRotation();
		Vec3 newRotation = Vec3(currentRotation.x, targetYaw * 180.0f / 3.14159f, currentRotation.z);
		m_playerObject->GetTransform()->SetLocalRotation(newRotation);
		m_circle->DamageFlag(true);
		m_moveFlag = true;
		SOUND->StopSound(17);
		SOUND->PlaySound(L"Bianca/Bianca_Skill03_Attack.wav", 17, 0.5f);
	}
	
	if (m_moveFlag) {
		if (m_moveElapsedTime <= m_moveDuration) {
			m_moveElapsedTime += DT;
			float movet = m_moveElapsedTime / m_moveDuration;
			Vec3 curPos = Utils::Lerp(m_startPos, m_targetPos, movet);
			m_playerObject->GetTransform()->SetPosition(curPos);
			m_circle->DamageFlag(true);
		}
		else {
			m_moveFlag = false;
			m_circleSizeElapedTime = 0.f;
			m_circleKeepElapedTime = 0.f;
			m_moveDuration = 0.f;
			m_moveElapsedTime = 0.f;
			m_startPos = m_playerObject->GetTransform()->GetPosition();

			auto objects = m_circle->GetCollisionObjects();

			//여기서 데미지 주기. 
			//SOUND->PlaySound(L"Bianca/Bianca_Skill03_Hit.wav", 17, 0.5f);
			//////아직 구현 X

			m_circle->DamageFlag(false);
			SkillEnd();
			m_circle->SetActive(false);
		}
	}
}
