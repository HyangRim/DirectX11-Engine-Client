#include "pch.h"
#include "NickyRSkill.h"

#include "Player.h"
#include "AnimationState.h"
#include "NickyRSkillState.h"

NickyRSkill::NickyRSkill(shared_ptr<Player> _player)
	: Super(_player)
{

}

NickyRSkill::~NickyRSkill()
{

}

void NickyRSkill::PlaySkill()
{
	if (!m_target)
	{
		cout << "R 스킬: 타겟이 설정되지 않았습니다." << endl;
		return;
	}

	int soundIdx = rand() % soundCount + 1;
	wstring soundString = L"Nicky/Nicky_PlaySkill4_" + to_wstring(soundIdx) + L".wav";

	SOUND->PlaySound(soundString, 1, 0.5f);
	SOUND->PlaySound(L"Nicky/Nicky_skill04_Ready.wav", 4, 0.5f);
	// 타겟 위치로 스킬 방향 계산
	CalculateSkillDirection();
}

void NickyRSkill::Update()
{
	//if (m_moveFlag)
	//{
	//	if (m_moveElapsedTime <= m_moveDuration && IsRushAnimationPlaying())
	//	{
	//		//cout << "움직여\n";
	//		m_moveElapsedTime += DT;
	//		float movet = m_moveElapsedTime / m_moveDuration;
	//		cout << "진행률 : " << movet * 100 << endl;
	//		Vec3 curPos = Utils::Lerp(m_startPos, m_targetPos, movet);
	//		m_playerObject->GetTransform()->SetPosition(curPos);
	//	}
	//	else
	//	{
	//		//m_moveFlag = false;
	//		//m_moveDuration = 0.f;
	//		//m_moveElapsedTime = 0.f;
	//		m_startPos = m_playerObject->GetTransform()->GetPosition();

	//		// 타겟 초기화
	//		//m_target = nullptr;
	//	}
	//}

	if (m_moveFlag)
	{
		// Rush 애니메이션이 재생 중일 때만 이동
		if (IsRushAnimationPlaying())
		{
			m_rushSoundDuration += DT;
			m_moveElapsedTime += DT;
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
				m_moveFlag = false; // 이동 완료
				cout << "R 스킬 이동 완료!" << endl;
				SOUND->PlaySound(L"Nicky/Nicky_skill04_Attack.wav", 3, 0.5f);
			}

			cout << "진행률 : " << moveT * 100 << "%" << endl;
			Vec3 curPos = Utils::Lerp(m_startPos, m_targetPos, moveT);
			m_playerObject->GetTransform()->SetPosition(curPos);
		}
		else if (m_moveElapsedTime > 0.0f)
		{
			// Rush 애니메이션이 끝났는데 이동 중이면 중지
			m_moveFlag = false;
			cout << "Rush 애니메이션 종료로 인한 이동 중지" << endl;
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
	
	SetRushDuration(m_moveDuration * 2);

	m_moveElapsedTime = 0.f;
	m_moveFlag = true;

	// 회전 계산 및 적용
	float targetYaw = atan2(direction.x, direction.z) + 3.141592f;
	Vec3 currentRotation = m_playerObject->GetTransform()->GetLocalRotation();
	Vec3 newRotation = Vec3(currentRotation.x, targetYaw * 180.0f / 3.14159f, currentRotation.z);

	m_playerObject->GetTransform()->SetLocalRotation(newRotation);

	cout << "R 스킬 방향 계산 완료 - 이동거리: " << distance << endl;

	cout << "m_moveElapsedTime : " << m_moveElapsedTime << ", m_moveDuration : " << m_moveDuration << endl;
}

// 첫 번째 애니메이션이 재생 중인지 확인하는 함수 추가
bool NickyRSkill::IsRushAnimationPlaying()
{
	shared_ptr<AnimationState> curAnimState = m_playerObject->GetAnimationStateMachine()->GetCurrentState2(AnimationStateType::Skill_4);

	return static_pointer_cast<NickyRSkillState>(curAnimState)->IsRushAnimationActive();

	//auto animator = m_playerObject->GetModelAnimator();
	//if (!animator) return false;

	//wstring currentAnim = animator->GetCurrentAnimationTag();

	//// 시퀀스가 재생 중이고, 현재 애니메이션이 Rush인 경우만 true
	//return animator->IsSequencePlaying() && currentAnim == L"Skill_01_Rush";
}

void NickyRSkill::SetRushDuration(float duration)
{
	m_playerObject->GetModelAnimator()->SetSequenceAnimationDuration(L"Skill_4_Sequence", 2, duration);
}
