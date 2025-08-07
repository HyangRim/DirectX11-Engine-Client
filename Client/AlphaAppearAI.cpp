#include "pch.h"
#include "AlphaAppearAI.h"
#include "Player.h"
#include "Monster.h"

AlphaAppearAI::AlphaAppearAI(shared_ptr<Monster> _Owner)
	: Super(_Owner)
{
}

AlphaAppearAI::~AlphaAppearAI()
{
}

void AlphaAppearAI::Enter()
{
	m_Owner->GetAnimationStateMachine()->ChangeState(AnimationStateType::Appear);
	m_Owner->SetMonsterState(MonsterState::APPEAR);

	wcout << L"Alpha Enter Appear AI\n";
}

void AlphaAppearAI::Update()
{
	if (m_AppearAnimElapsedTime >= m_AppearAnimDuration) {
		m_Owner->ChangeState(L"Idle");
	}
	m_AppearAnimElapsedTime += DT;
}

void AlphaAppearAI::Exit()
{
	m_AppearAnimElapsedTime = 0.f;
}
