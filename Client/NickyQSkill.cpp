#include "pch.h"
#include "NickyQSkill.h"

#include "Player.h"
#include "NavMeshAgent.h"

NickyQSkill::NickyQSkill(shared_ptr<Player> _player)
	: Super(_player)
{

}

NickyQSkill::~NickyQSkill()
{

}

void NickyQSkill::PlaySkill()
{

}

void NickyQSkill::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::Q))
	{
		m_bskillStart = true;
		m_duration = 0.f;
	}

	if (m_bskillStart)
	{
		m_duration += DT;
	}

	if (INPUT->GetButtonUp(KEY_TYPE::Q))
	{
		m_playerObject->GetNavMeshAgent()->Stop();
		CalculateSkillDirection();
		m_duration = 0.f;
	}

	if (m_moveFlag)
	{
		if (m_moveElapsedTime <= m_moveDuration && IsFirstAnimationPlaying())
		{
			m_moveElapsedTime += DT;
			float movet = m_moveElapsedTime / m_moveDuration;
			Vec3 curPos = Utils::Lerp(m_startPos, m_targetPos, movet);
			m_playerObject->GetTransform()->SetPosition(curPos);
		}
		else
		{
			// 첫 번째 애니메이션이 끝나면 이동 중지
			m_moveFlag = false;
			m_moveDuration = 0.f;
			m_moveElapsedTime = 0.f;
			m_startPos = m_playerObject->GetTransform()->GetPosition();
		}
	}

}


void NickyQSkill::CalculateSkillDirection()
{
	float range = 10.f;

	POINT mousePos = INPUT->GetMousePos();
	//::ScreenToClient(GAME->GetGameDesc().hWnd, &mousePos);

	XMVECTOR mouseWorldPos = ScreenToWorld(mousePos);

	XMVECTOR playerPos = m_playerObject->GetTransform()->GetPosition();
	XMVECTOR direction = XMVector3Normalize(mouseWorldPos - playerPos);

	XMVECTOR skillTargetPos = XMVectorAdd(playerPos, XMVectorScale(direction, range));
	m_startPos = m_playerObject->GetTransform()->GetPosition();
	m_targetPos = skillTargetPos;

	float distance = Vec3::Distance(m_startPos, m_targetPos);
	m_moveDuration = (distance / m_speed);
	m_moveElapsedTime = 0.f;
	m_moveFlag = true;

	// 회전 계산 및 적용
	float targetYaw = atan2(XMVectorGetX(direction), XMVectorGetZ(direction)) + 3.141592f; //3.141592 더해야 방향 제대로 됨

	//cout << "TargetYaw : " << targetYaw * 57.2958f << "\n";
	Vec3 currentRotation = m_playerObject->GetTransform()->GetLocalRotation();
	Vec3 newRotation = Vec3(currentRotation.x, targetYaw * 180.0f / 3.14159f, currentRotation.z);

	m_playerObject->GetTransform()->SetLocalRotation(newRotation);
}

// 첫 번째 애니메이션이 재생 중인지 확인하는 함수 추가
bool NickyQSkill::IsFirstAnimationPlaying()
{
	auto animator = m_playerObject->GetModelAnimator();
	if (!animator) return false;

	wstring currentAnim = animator->GetCurrentAnimationTag();

	// 시퀀스가 재생 중이고, 현재 애니메이션이 Rush인 경우만 true
	return animator->IsSequencePlaying() && currentAnim == L"Skill_01_Rush";
}