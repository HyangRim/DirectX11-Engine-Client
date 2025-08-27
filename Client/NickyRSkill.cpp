#include "pch.h"
#include "NickyRSkill.h"

#include "Player.h"
#include "AnimationState.h"
#include "NickyAnimRState.h"
#include "Monster.h"

NickyRSkill::NickyRSkill(shared_ptr<Player> _player)
	: Super(_player, 3)
{
	m_skillCooldown = 2.f;

	SetMaxSkillLevel(3);

	m_skillImage = RESOURCES->GetOrAddTexture(L"NickyR", L"..\\Resources\\Textures\\UI\\SkillIcon\\SkillIcon_1033500.png");
}

NickyRSkill::~NickyRSkill()
{

}

void NickyRSkill::PlaySkill()
{
	//if (!m_target)
	//{
	//	cout << "R 스킬: 타겟이 설정되지 않았습니다." << endl;
	//	return;
	//}

	//int soundIdx = rand() % soundCount + 1;
	//wstring soundString = L"Nicky/Nicky_PlaySkill4_" + to_wstring(soundIdx) + L".wav";

	//SOUND->PlaySound(soundString, 1, 0.5f);
	//SOUND->PlaySound(L"Nicky/Nicky_skill04_Ready.wav", 4, 0.5f);
	//// 타겟 위치로 스킬 방향 계산
	//CalculateSkillDirection();

	if (!m_target)
	{
		cout << "R 스킬: 타겟이 설정되지 않았습니다." << endl;
		return;
	}

	// 초기 타겟 위치 저장
	m_lastTargetPos = m_target->GetTransform()->GetPosition();

	int soundIdx = rand() % soundCount + 1;
	wstring soundString = L"Nicky/Nicky_PlaySkill4_" + to_wstring(soundIdx) + L".wav";
	SOUND->PlaySound(soundString, 1, 0.5f);
	SOUND->PlaySound(L"Nicky/Nicky_skill04_Ready.wav", 4, 0.5f);

	CalculateSkillDirection();
}

void NickyRSkill::Update()
{
	//UpdateSkillCoolDown();

	//if (m_moveFlag)
	//{
	//	// Rush 애니메이션이 재생 중일 때만 이동
	//	if (IsRushAnimationPlaying())
	//	{
	//		m_rushSoundDuration += DT;
	//		m_moveElapsedTime += DT;
	//		float moveT = m_moveElapsedTime / m_moveDuration;

	//		if (m_rushSoundDuration > 0.1f)
	//		{
	//			SOUND->PlaySound(L"Nicky/Nicky_skill04_Rush.wav", 2, 0.5f);
	//			m_rushSoundDuration = 0.f;
	//		}

	//		// 이동 완료 체크
	//		if (moveT >= 1.0f)
	//		{
	//			moveT = 1.0f;
	//			m_moveFlag = false; // 이동 완료
	//			//cout << "R 스킬 이동 완료!" << endl;
	//			SOUND->PlaySound(L"Nicky/Nicky_skill04_Attack.wav", 3, 0.5f);

	//			static_pointer_cast<Monster>(m_target)->Damaged(m_playerObject,
	//				static_pointer_cast<Player>(m_playerObject)->GetStatus().hitAttack * 1.5f);

	//			SkillEnd();
	//		}

	//		//cout << "진행률 : " << moveT * 100 << "%" << endl;
	//		Vec3 curPos = Utils::Lerp(m_startPos, m_targetPos, moveT);
	//		m_playerObject->GetTransform()->SetPosition(curPos);
	//	}
	//	else if (m_moveElapsedTime > 0.0f)
	//	{
	//		// Rush 애니메이션이 끝났는데 이동 중이면 중지
	//		m_moveFlag = false;
	//	}
	//}


	UpdateSkillCoolDown();

	if (m_moveFlag)
	{
		if (IsRushAnimationPlaying())
		{
			m_rushSoundDuration += DT;
			m_moveElapsedTime += DT;

			// **핵심**: 동적 타겟 추적 로직
			if (m_isDynamicTracking && m_target)
			{
				m_trackingTimer += DT;

				// 일정 간격으로만 업데이트 (렉 방지)
				if (m_trackingTimer >= m_trackingUpdateInterval)
				{
					UpdateTargetPosition();
					m_trackingTimer = 0.f;
				}
			}

			float moveT = m_moveElapsedTime / m_moveDuration;

			if (m_rushSoundDuration > 0.1f)
			{
				SOUND->PlaySound(L"Nicky/Nicky_skill04_Rush.wav", 2, 0.5f);
				m_rushSoundDuration = 0.f;
			}

			// 이동 완료 체크
			if (moveT >= 1.0f)
			{
				moveT = 1.0f;
				m_moveFlag = false;
				SOUND->PlaySound(L"Nicky/Nicky_skill04_Attack.wav", 3, 0.5f);

				if (m_target && m_target->GetType() == OBJECTTYPE::MONSTER)
				{
					static_pointer_cast<Monster>(m_target)->Damaged(m_playerObject,
						static_pointer_cast<Player>(m_playerObject)->GetStatus().hitAttack * 1.5f);
				}

				SkillEnd();
			}

			Vec3 curPos = Utils::Lerp(m_startPos, m_targetPos, moveT);
			m_playerObject->GetTransform()->SetPosition(curPos);
		}
		else if (m_moveElapsedTime > 0.0f)
		{
			m_moveFlag = false;
		}
	}
}

void NickyRSkill::CalculateSkillDirection()
{
	if (!m_target)
	{
		cout << "타겟이 없어서 방향 계산 불가" << endl;
		return;
	}

	// 타겟 위치로 방향 설정
	Vec3 playerPos = m_playerObject->GetTransform()->GetPosition();
	Vec3 targetPos = m_target->GetTransform()->GetPosition();

	Vec3 direction = targetPos - playerPos;
	direction.Normalize();

	// 적절한 거리로 이동 (타겟 바로 앞까지)
	float moveDistance = Vec3::Distance(playerPos, targetPos) - 1.f; // 1미터 앞에서 정지
	if (moveDistance < 0) moveDistance = 1.0f; // 최소 이동거리

	m_startPos = playerPos;
	m_targetPos = playerPos + direction * moveDistance;

	float distance = Vec3::Distance(m_startPos, m_targetPos);
	m_moveDuration = distance / m_speed;
	
	SetRushDuration(m_moveDuration);
	cout << "러쉬 시간 : " << m_moveDuration << endl;

	m_moveElapsedTime = 0.f;
	m_moveFlag = true;

	// 회전 계산 및 적용
	float targetYaw = atan2(direction.x, direction.z) + 3.141592f;
	Vec3 currentRotation = m_playerObject->GetTransform()->GetLocalRotation();
	Vec3 newRotation = Vec3(currentRotation.x, targetYaw * 180.0f / 3.14159f, currentRotation.z);

	m_playerObject->GetTransform()->SetLocalRotation(newRotation);
}

// 첫 번째 애니메이션이 재생 중인지 확인하는 함수 추가
bool NickyRSkill::IsRushAnimationPlaying()
{
	shared_ptr<AnimationState> curAnimState = m_playerObject->GetAnimationStateMachine()->GetState(AnimationStateType::Skill_4);

	return static_pointer_cast<NickyAnimRState>(curAnimState)->IsRushAnimationActive();

	return false;
}

void NickyRSkill::SetRushDuration(float duration)
{
	m_playerObject->GetModelAnimator()->SetSequenceAnimationDuration(L"Skill_4_Sequence", 2, duration);
}

void NickyRSkill::UpdateTargetPosition()
{
	if (!m_target || !m_target->GetActive())
		return;

	Vec3 currentTargetPos = m_target->GetTransform()->GetPosition();

	// 타겟이 실제로 이동했는지 확인 (불필요한 계산 방지)
	float distanceMoved = Vec3::Distance(currentTargetPos, m_lastTargetPos);
	if (distanceMoved < 0.1f)  // 0.1미터 이하 움직임은 무시
		return;

	Vec3 playerPos = m_playerObject->GetTransform()->GetPosition();
	Vec3 direction = currentTargetPos - playerPos;
	direction.Normalize();

	// 타겟 바로 앞 1미터까지만 이동
	float moveDistance = Vec3::Distance(playerPos, currentTargetPos) - 1.f;
	if (moveDistance < 0.5f) moveDistance = 0.5f;  // 최소 거리 보장

	// **부드러운 전환**: 기존 목표점과 새 목표점을 보간
	Vec3 newTargetPos = playerPos + direction * moveDistance;
	m_targetPos = Utils::Lerp(m_targetPos, newTargetPos, 0.3f);  // 30% 보간으로 부드럽게

	// 회전도 업데이트
	float targetYaw = atan2(direction.x, direction.z) + 3.141592f;
	Vec3 currentRotation = m_playerObject->GetTransform()->GetLocalRotation();
	Vec3 newRotation = Vec3(currentRotation.x, targetYaw * 180.0f / 3.14159f, currentRotation.z);
	m_playerObject->GetTransform()->SetLocalRotation(newRotation);

	m_lastTargetPos = currentTargetPos;  // 이전 위치 저장
}