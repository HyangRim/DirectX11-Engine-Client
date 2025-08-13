#include "pch.h"
#include "SkillLevelUpPanelUI.h"

#include "Player.h"

SkillLevelUpPanelUI::SkillLevelUpPanelUI(shared_ptr<Player> player)
	:m_player(player)
{

}

SkillLevelUpPanelUI::~SkillLevelUpPanelUI()
{

}


void SkillLevelUpPanelUI::Initialize()
{
	LoadResources();
	CreatePanels();
}

void SkillLevelUpPanelUI::Update()
{
	UpdateSkillLevelPanel();
}

void SkillLevelUpPanelUI::SetVisible(bool visible)
{
	m_isVisible = visible;
	m_panel->GetUIPanel()->SetVisible(visible);
}

void SkillLevelUpPanelUI::Cleanup()
{
	
}

void SkillLevelUpPanelUI::LoadResources()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	//스킬 레벨업 이미지
	shared_ptr<Material> btn_SkillLevelUp = make_shared<Material>();
	SetupUIMaterial(btn_SkillLevelUp);

	wstring tag = L"Btn_LevelUp_MouseOver";
	wstring path = L"..\\Resources\\Textures\\UI\\status\\" + tag + L".png";
	auto btn_SkillLevelUpTexture = RESOURCES->Load<Texture>(tag, path);

	btn_SkillLevelUp->SetDiffuseMap(btn_SkillLevelUpTexture);
	MaterialDesc& btn_SkillLevelUpDesc = btn_SkillLevelUp->GetMaterialDesc();
	btn_SkillLevelUpDesc.ambient = Vec4(1.f);
	btn_SkillLevelUpDesc.diffuse = Vec4(1.f);
	btn_SkillLevelUpDesc.specular = Vec4(1.f);
	RESOURCES->Add(tag, btn_SkillLevelUp);
}

void SkillLevelUpPanelUI::CreatePanels()
{
	m_panel = make_shared<GameObject>();
	m_panel->SetName(L"SkillLevelUpPanel");

	auto panel = make_shared<UIPanel>();
	m_panel->AddComponent(panel);

	panel->Create(Vec2(602.f, 768 - 57 - 85), Vec2(153, 40), Vec4(1.f, 1.f, 1.f, 0.7f), nullptr);
	m_panel->SetLayerIndex(LAYER_UI);

	shared_ptr<Material> cloneMaterial_skillLevelUpBtn = RESOURCES->Get<Material>(L"Btn_LevelUp_MouseOver");
	for (int i = 0; i < 4; i++)
	{
		auto skillLevelUpBtn = panel->AddButton(Vec2(10 + 43 * i, 25), Vec2(48, 52), cloneMaterial_skillLevelUpBtn->Clone(), L"SkillLevelUpBtn" + to_wstring(i));
		skillLevelUpBtn->OnClick += [this, i] {
			m_player->GetSkill(i)->SkillLevelUp();
			m_player->GetStatus().availableSkillPoints--;
		};
	}

	CURSCENE->AddUIObject(m_panel, true);
	CURSCENE->RegisterUIParent(m_panel);
}

void SkillLevelUpPanelUI::RegisterUIObject(shared_ptr<GameObject> uiObject)
{

}

void SkillLevelUpPanelUI::UpdateSkillLevelPanel()
{
	PlayerStatus& playerStatus = m_player->GetStatus();

	if (playerStatus.availableSkillPoints > 0) SetVisible(true);
	else SetVisible(false);

	if (m_player->GetSkill(0)->GetCurSkillLevel() == m_player->GetSkill(0)->GetMaxSkillLevel())
	{
		m_panel->GetUIPanel()->GetButton(L"SkillLevelUpBtn" + to_wstring(0))->SetVisible(false);
	}
	if (m_player->GetSkill(1)->GetCurSkillLevel() == m_player->GetSkill(1)->GetMaxSkillLevel())
	{
		m_panel->GetUIPanel()->GetButton(L"SkillLevelUpBtn" + to_wstring(1))->SetVisible(false);
	}
	if (m_player->GetSkill(2)->GetCurSkillLevel() == m_player->GetSkill(2)->GetMaxSkillLevel())
	{
		m_panel->GetUIPanel()->GetButton(L"SkillLevelUpBtn" + to_wstring(2))->SetVisible(false);
	}
	if (m_player->GetSkill(3)->GetCurSkillLevel() == m_player->GetSkill(3)->GetMaxSkillLevel())
	{
		m_panel->GetUIPanel()->GetButton(L"SkillLevelUpBtn" + to_wstring(3))->SetVisible(false);
	}
}