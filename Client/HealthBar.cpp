#include "pch.h"
#include "HealthBar.h"

#include "UIPanel.h"
#include "ImageUI.h"
#include "Player.h"

HealthBar::HealthBar()
{
	m_barSize = Vec2(60.f, 8.f);
	m_offset = Vec3(0.f, 6.f, 0.f);
}

HealthBar::~HealthBar()
{
}

void HealthBar::Start()
{
	Super::Start();
	Create();
}

void HealthBar::Update()
{
	Super::Update();
	UpdateHealthBarPosition();
}

void HealthBar::Create(Vec3 _offset)
{
	m_offset = _offset;

	//UI Panel 생성. 
	auto panelObj = make_shared<GameObject>();
	panelObj->SetName(L"HealthBarPanel_" + GetGameObject()->GetName());

	m_healthBarPanel = make_shared<UIPanel>();
	panelObj->AddComponent(m_healthBarPanel);

	m_healthBarPanel->Create(Vec2(100, 100), Vec2(70, 20), nullptr);

	auto backgroundMaterial = make_shared<Material>();
	auto shader = make_shared<Shader>(L"ImageShader.fx");
	backgroundMaterial->SetShader(shader);
	backgroundMaterial->SetRenderQueue(RenderQueue::Transparent);
	backgroundMaterial->SetTransparent(true);

	MaterialDesc& bgDesc = backgroundMaterial->GetMaterialDesc();
	bgDesc.diffuse = Vec4(0.2f, 0.2f, 0.2f, 0.8f);

	m_healthBarUI = m_healthBarPanel->AddImageUI(Vec2(0, 0), L"HealthBar");
	m_healthBarUI->AddImageLayer(0, Vec2(0, 0), m_barSize, backgroundMaterial, 2);

	auto healthMaterial = make_shared<Material>();
	healthMaterial->SetShader(shader);
	healthMaterial->SetRenderQueue(RenderQueue::Transparent);
	healthMaterial->SetTransparent(true);

	MaterialDesc& healthDesc = healthMaterial->GetMaterialDesc();
	healthDesc.diffuse = Vec4(0.2f, 0.8f, 0.2f, 1.f);

	m_healthBarUI->AddImageLayer(1, Vec2(0, 0), m_barSize, healthMaterial, 2);

	CURSCENE->AddUIObject(panelObj, true);
	CURSCENE->RegisterUIParent(panelObj);
}

void HealthBar::UpdateHealth(int _curHP, int _maxHP)
{
	if (_curHP == m_lastCurHP && _maxHP == m_lastMaxHP)
		return;

	m_lastCurHP = _curHP;
	m_lastMaxHP = _maxHP;

	if (m_lastMaxHP <= 0)return;

	float healthRatio = static_cast<float>(_curHP) / static_cast<float>(_maxHP);
	healthRatio = max(0.f, min(healthRatio, 1.f));
	
	UpdateHealthBarSize(healthRatio);
}

void HealthBar::SetVisible(bool _visible)
{
	if (m_healthBarPanel) {
		m_healthBarPanel->SetVisible(_visible);
	}
}

void HealthBar::UpdateHealthBarPosition()
{
	Vec3 targetPos = GetTransform()->GetPosition();

	if (Vec3::Distance(targetPos, m_lastTargetPos) < 0.1f)
		return;

	m_lastTargetPos = targetPos;

	Vec3 healthBarWorldPos = targetPos + m_offset;

	auto camera = CURSCENE->GetMainCamera()->GetCamera();
	Matrix worldMatrix = Matrix::Identity;
	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix projMatrix = camera->GetProjectionMatrix();

	Vec3 screenPos = GRAPHICS->GetViewport().Project(healthBarWorldPos,
		worldMatrix, viewMatrix, projMatrix);

	if (m_healthBarPanel) {
		m_healthBarPanel->SetPosition(Vec2(screenPos.x, screenPos.y));
	}
}

void HealthBar::UpdateHealthBarSize(float _healthRatio)
{
	if (!m_healthBarUI) return;

	Vec2 currentHealthBarSize = Vec2(m_barSize.x * _healthRatio, m_barSize.y);
	m_healthBarUI->SetLayerSize(1, currentHealthBarSize);

	// 왼쪽 정렬
	float offsetX = -(m_barSize.x - currentHealthBarSize.x) / 2.0f;
	m_healthBarUI->SetLayerPosition(1, Vec2(offsetX, 0));
}

