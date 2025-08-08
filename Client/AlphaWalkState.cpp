#include "pch.h"
#include "AlphaWalkState.h"

AlphaWalkState::AlphaWalkState()
	:Super(MonsterStateType::Move)
{

}

void AlphaWalkState::Enter()
{
	m_animTime = 0.f;
	m_isAnimationStarted = true;

	cout << "알파 Wait State 진입\n";
}

void AlphaWalkState::Update()
{

}

void AlphaWalkState::Exit()
{
	m_animTime = 0.f;
	m_isAnimationStarted = false;

	cout << "알파 Wait State 종료\n";
}

bool AlphaWalkState::CanTransitionTo(MonsterStateType newState)
{
	if (newState == MonsterStateType::Wait)
		return true;
	return false;
}