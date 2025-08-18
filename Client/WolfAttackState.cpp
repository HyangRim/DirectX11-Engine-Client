#include "pch.h"
#include "WolfAttackState.h"


WolfAttackState::WolfAttackState()
	:Super(MonsterStateType::Attack)
{

}

void WolfAttackState::Enter()
{
	m_animTime = 0.f;
	m_isAnimationStarted = true;
	//m_isAppearComplete = false;



	cout << "늑대 Appear State 진입\n";
}

void WolfAttackState::Update()
{
	m_animTime += DT;
	/*if (!m_isAppearComplete && m_animTime >= (60.f / 25.f) / 2.f)
	{
		cout << "늑대 Appear 애니메이션 완료!" << endl;
		m_isAppearComplete = true;
	}*/
}

void WolfAttackState::Exit()
{
	m_animTime = 0.f;
	m_isAnimationStarted = false;

	cout << "늑대 Appear State 종료\n";
}

bool WolfAttackState::CanTransitionTo(MonsterStateType newState)
{
	/*if (m_isAppearComplete && newState == MonsterStateType::Wait)
		return true;
	return false;*/
	return true;
}
