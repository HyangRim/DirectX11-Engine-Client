#include "pch.h"
#include "SceneManager.h"

void SceneManager::Update()
{
	if (m_curScene == nullptr)
		return;

	m_curScene->Update();
	m_curScene->FixedUpdate();
	m_curScene->LateUpdate();

	m_curScene->Render();
}
