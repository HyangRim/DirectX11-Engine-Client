#include "pch.h"
#include "BaseSkill.h"
#include "Player.h"

BaseSkill::BaseSkill(shared_ptr<Player> _player)
	: m_playerObject(_player)
{

}

BaseSkill::~BaseSkill()
{

}

void BaseSkill::Update()
{

}

void BaseSkill::PlaySkill()
{

}

void BaseSkill::SkillEnd()
{
	m_skillcurCooldown = m_skillCooldown * (1 - m_playerObject->GetStatus().cooldownReduction);
}

XMVECTOR BaseSkill::ScreenToWorld(POINT _screenPos)
{
	// 올바른 NDC (Normalized Device Coordinates) 변환
	float x = (2.0f * _screenPos.x) / GRAPHICS->GetViewport().GetWidth() - 1.0f;
	float y = 1.0f - (2.0f * _screenPos.y) / GRAPHICS->GetViewport().GetHeight(); // Height 사용

	// Near와 Far 평면의 점을 NDC에서 정의
	XMVECTOR rayOrigin = XMVectorSet(x, y, 0.0f, 1.0f);  // Near plane
	XMVECTOR rayEnd = XMVectorSet(x, y, 1.0f, 1.0f);     // Far plane

	// ViewProjection 역행렬 계산
	XMMATRIX viewMatrix = CURSCENE->GetMainCamera()->GetCamera()->GetViewMatrix();
	XMMATRIX projMatrix = CURSCENE->GetMainCamera()->GetCamera()->GetProjectionMatrix();
	XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewMatrix * projMatrix);

	// NDC에서 월드 좌표로 변환
	rayOrigin = XMVector3TransformCoord(rayOrigin, invViewProj);
	rayEnd = XMVector3TransformCoord(rayEnd, invViewProj);

	// 레이 방향 계산
	XMVECTOR rayDir = XMVector3Normalize(rayEnd - rayOrigin);

	// 플레이어와 같은 높이 평면에 투영
	float playerY = m_playerObject->GetTransform()->GetPosition().y;
	float t = (playerY - XMVectorGetY(rayOrigin)) / XMVectorGetY(rayDir);

	// 최종 월드 좌표 계산
	XMVECTOR worldPos = rayOrigin + rayDir * t;
	return worldPos;
}
