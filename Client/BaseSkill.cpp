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
    float x = (2.0f * _screenPos.x) / GRAPHICS->GetViewport().GetWidth() - 1.0f;
    float y = 1.0f - (2.0f * _screenPos.y) / GRAPHICS->GetViewport().GetWidth();

    XMVECTOR rayOrigin = XMVectorSet(x, y, 0.0f, 1.0f);
    XMVECTOR rayEnd = XMVectorSet(x, y, 1.0f, 1.0f);

    XMMATRIX invViewProj = XMMatrixInverse(nullptr,
        CURSCENE->GetMainCamera()->GetCamera()->GetViewMatrix() * CURSCENE->GetMainCamera()->GetCamera()->GetProjectionMatrix());

    rayOrigin = XMVector3TransformCoord(rayOrigin, invViewProj);
    rayEnd = XMVector3TransformCoord(rayEnd, invViewProj);

    // 플레이어와 같은 높이 평면에 투영
    XMVECTOR rayDir = XMVector3Normalize(rayEnd - rayOrigin);
    float playerY = m_playerObject->GetTransform()->GetPosition().y;
    float t = (playerY - rayOrigin.m128_f32[1]) / rayDir.m128_f32[1];

    return rayOrigin + rayDir * t;
}
