#include "pch.h"
#include "GameHUDPanelUI.h"

#include "Player.h"

const vector<wstring> nickySkillIcons = {
	L"SkillIcon_1033100",
	L"SkillIcon_1033200",
	L"SkillIcon_1033300",
	L"SkillIcon_1033400",
	L"SkillIcon_1033500"
};

const vector<wstring> biancaSkillIcons = {
	L"SkillIcon_1042100",
	L"SkillIcon_1042200",
	L"SkillIcon_1042300",
	L"SkillIcon_1042400",
	L"SkillIcon_1042500"
};

GameHUDPanelUI::GameHUDPanelUI(shared_ptr<Player> player, int selectedCharIdx)
	: m_player(player)
	, m_selectedCharIdx(selectedCharIdx)
{

}

GameHUDPanelUI::~GameHUDPanelUI()
{
	
}

void GameHUDPanelUI::Initialize()
{
	LoadResources();
	CreatePanels();
}

void GameHUDPanelUI::Update()
{
	UpdateSkillCoolDown();
}

void GameHUDPanelUI::SetVisible(bool visible)
{
	m_isVisible = visible;
	m_panel->GetUIPanel()->SetVisible(visible);
}

void GameHUDPanelUI::Cleanup()
{

}

void GameHUDPanelUI::LoadResources()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
	};

	wstring prefixPath = L"..\\Resources\\Textures\\UI\\SkillIcon\\";
	vector<wstring> skillTag = { L"P", L"Q", L"W", L"E", L"R" };
	//니키 스킬 아이콘
	for (int i = 0; i < nickySkillIcons.size(); i++)
	{
		shared_ptr<Material> charSkillIcon = make_shared<Material>();
		SetupUIMaterial(charSkillIcon);

		wstring tag = L"Nicky" + skillTag[i];
		wstring path = prefixPath + nickySkillIcons[i] + L".png";
		auto charSkillIconTexture = RESOURCES->Load<Texture>(tag, path);

		charSkillIcon->SetDiffuseMap(charSkillIconTexture);
		MaterialDesc& charSkillIconDesc = charSkillIcon->GetMaterialDesc();
		charSkillIconDesc.ambient = Vec4(1.f);
		charSkillIconDesc.diffuse = Vec4(1.f);
		charSkillIconDesc.specular = Vec4(1.f);
		RESOURCES->Add(tag, charSkillIcon);
	}


	//비앙카 스킬 아이콘
	for (int i = 0; i < biancaSkillIcons.size(); i++)
	{
		shared_ptr<Material> charSkillIcon = make_shared<Material>();
		SetupUIMaterial(charSkillIcon);

		wstring tag = L"Bianca" + skillTag[i];
		wstring path = prefixPath + biancaSkillIcons[i] + L".png";
		auto charSkillIconTexture = RESOURCES->Load<Texture>(tag, path);

		charSkillIcon->SetDiffuseMap(charSkillIconTexture);
		MaterialDesc& charSkillIconDesc = charSkillIcon->GetMaterialDesc();
		charSkillIconDesc.ambient = Vec4(1.f);
		charSkillIconDesc.diffuse = Vec4(1.f);
		charSkillIconDesc.specular = Vec4(1.f);
		RESOURCES->Add(tag, charSkillIcon);
	}

	//hp 이미지
	shared_ptr<Material> charHpBar = make_shared<Material>();
	SetupUIMaterial(charHpBar);

	wstring tag = L"HPBar_UI";
	wstring path = L"..\\Resources\\Textures\\UI\\status\\" + tag + L".png";
	auto charHpBarTexture = RESOURCES->Load<Texture>(tag, path);

	charHpBar->SetDiffuseMap(charHpBarTexture);
	MaterialDesc& charHpBarDesc = charHpBar->GetMaterialDesc();
	charHpBarDesc.ambient = Vec4(1.f);
	charHpBarDesc.diffuse = Vec4(1.f);
	charHpBarDesc.specular = Vec4(1.f);
	RESOURCES->Add(tag, charHpBar);

	//sp 이미지
	shared_ptr<Material> charSpBar = make_shared<Material>();
	SetupUIMaterial(charSpBar);

	tag = L"SPBar_UI";
	path = L"..\\Resources\\Textures\\UI\\status\\" + tag + L".png";
	auto charSpBarTexture = RESOURCES->Load<Texture>(tag, path);

	charSpBar->SetDiffuseMap(charSpBarTexture);
	MaterialDesc& charSpBarDesc = charSpBar->GetMaterialDesc();
	charSpBarDesc.ambient = Vec4(1.f);
	charSpBarDesc.diffuse = Vec4(1.f);
	charSpBarDesc.specular = Vec4(1.f);
	RESOURCES->Add(tag, charSpBar);
}

void GameHUDPanelUI::CreatePanels()
{
	m_panel = make_shared<GameObject>();
	m_panel->SetName(L"GameHUDPanel");

	auto panel = make_shared<UIPanel>();
	m_panel->AddComponent(panel);

	panel->Create(Vec2(615.f, 768 - 57), Vec2(414, 115), Vec4(1.f, 0.f, 1.f, 0.7f), nullptr);
	m_panel->SetLayerIndex(LAYER_UI);


	wstring characterTag = L"";
	if (m_selectedCharIdx == 0) characterTag = L"Bianca";
	else if (m_selectedCharIdx == 1) characterTag = L"Nicky";

	auto imageUI = m_panel->GetUIPanel()->AddImageUI(Vec2(0, 0), L"ImageUI");

	vector<wstring> skillTag = { L"Q", L"W", L"E", L"R" };
	for (int i = 0; i < 4; i++)
	{
		shared_ptr<Material> cloneMaterial_skillIcon = RESOURCES->Get<Material>(characterTag + skillTag[i])->Clone();
		imageUI->AddImageLayer(i, Vec2(128 + 43 * i, 25), Vec2(35, 38), cloneMaterial_skillIcon, 1);
	}

	//Q
	auto textQ = panel->AddD2DText(Vec2(128, 25), L"5", 20.0f,
		Vec4(1, 0, 0, 1), 1.0f, Vec4(0, 0, 0, 0), 1.0f,
		L"QSkillCoolDown", TextAlignment::Center);
	textQ->SetUpdateInterval(1.f);

	//W
	auto textW = panel->AddD2DText(Vec2(128 + 43 * 1, 25), L"4", 20.0f,
		Vec4(1, 0, 0, 1), 1.0f, Vec4(0, 0, 0, 0), 1.0f,
		L"WSkillCoolDown", TextAlignment::Center);
	textW->SetUpdateInterval(1.f);

	//E
	auto textE = panel->AddD2DText(Vec2(128 + 43 * 2, 25), L"3", 20.0f,
		Vec4(1, 0, 0, 1), 1.0f, Vec4(0, 0, 0, 0), 1.0f,
		L"ESkillCoolDown", TextAlignment::Center);
	textE->SetUpdateInterval(1.f);

	//R
	auto textR = panel->AddD2DText(Vec2(128 + 43 * 3, 25), L"2", 20.0f,
		Vec4(1, 0, 0, 1), 1.0f, Vec4(0, 0, 0, 0), 1.0f,
		L"RSkillCoolDown", TextAlignment::Center);
	textR->SetUpdateInterval(1.f);


	//HP바 UI
	auto hpPanel = panel->AddPanel(Vec2(194, 70.f), Vec2(153, 10), nullptr, L"ChildHPPanel");
	hpPanel->AddD2DText(
		Vec2(153, 10) / 2.f,
		L"",
		10.f,
		Vec4(1.f, 1.f, 1.f, 1.f),
		1.f,
		Vec4(0.f),
		0.f,
		L"HPText",
		TextAlignment::Center
	);
	auto hpPanelImageUI = hpPanel->AddImageUI(Vec2(0.f), L"HPPanelImageUI");
	hpPanelImageUI->AddImageLayer(0, Vec2(153, 10) / 2.f, Vec2(153, 10) * (1 / RESOLUTION_CONSTANT), RESOURCES->Get<Material>(L"HPBar_UI")->Clone(), 1);

	Vec3 hpPos = hpPanelImageUI->GetGameObject()->GetTransform()->GetPosition();


	//SP바 UI
	auto spPanel = panel->AddPanel(Vec2(194, 85.f), Vec2(153, 10), nullptr, L"ChildSPPanel");
	spPanel->AddD2DText(
		Vec2(153, 10) / 2.f,
		L"",
		10.f,
		Vec4(1.f, 1.f, 1.f, 1.f),
		1.f,
		Vec4(0.f),
		0.f,
		L"SPText",
		TextAlignment::Center
	);
	auto spPanelImageUI = spPanel->AddImageUI(Vec2(0.f), L"SPPanelImageUI");
	spPanelImageUI->AddImageLayer(0, Vec2(153, 10) / 2.f, Vec2(153, 10) * (1 / RESOLUTION_CONSTANT), RESOURCES->Get<Material>(L"SPBar_UI")->Clone(), 1);

	//경험치바 UI
	auto expPanel = panel->AddPanel(Vec2(194, 100.f), Vec2(153, 10), nullptr, L"ChildEXPPanel");
	expPanel->AddD2DText(
		Vec2(153, 10) / 2.f,
		L"",
		10.f,
		Vec4(1.f, 1.f, 1.f, 1.f),
		1.f,
		Vec4(0.f),
		0.f,
		L"EXPText",
		TextAlignment::Center
	);
	auto expPanelImageUI = expPanel->AddImageUI(Vec2(0.f), L"EXPPanelImageUI");
	expPanelImageUI->AddImageLayer(0, Vec2(153, 10) / 2.f, Vec2(153, 10) * (1 / RESOLUTION_CONSTANT), RESOURCES->Get<Material>(L"HPBar_UI")->Clone(), 1);


	//캐릭터 이미지 패널 + 레벨
	//캐릭터 초상화
	shared_ptr<Material> cloneMaterial_charLobbyImage;
	if (m_selectedCharIdx == 0) cloneMaterial_charLobbyImage = RESOURCES->Get<Material>(L"CharLobbyBianca");
	else if (m_selectedCharIdx == 1) cloneMaterial_charLobbyImage = RESOURCES->Get<Material>(L"CharLobbyNicky");

	auto charImagePanel = panel->AddPanel(Vec2(58.f, 58.f), Vec2(100, 100), cloneMaterial_charLobbyImage, L"CharImagePanel");
	auto charLevelPanel = charImagePanel->AddPanel(Vec2(10.f, 80), Vec2(30.f, 30.f), nullptr, L"LevelPanel");
	Vec3 pos = charLevelPanel->GetGameObject()->GetTransform()->GetPosition();
	charLevelPanel->GetGameObject()->GetTransform()->SetPosition(Vec3(pos.x, pos.y, pos.z - 0.01));

	charLevelPanel->AddD2DText(
		Vec2(15.f, 15.f),
		L"20",
		12.f,
		Vec4(1.f),
		1.f,
		Vec4(0.f),
		0.f,
		L"LevelText",
		TextAlignment::Center
	);


	/*m_skillLevelUpPanel = make_shared<GameObject>();
	m_skillLevelUpPanel->SetName(L"SkillLevelUpPanel");

	auto skillPanel = make_shared<UIPanel>();
	m_skillLevelUpPanel->AddComponent(skillPanel);

	skillPanel->Create(Vec2(602.f, 768 - 57 - 85), Vec2(153, 40), Vec4(1.f, 1.f, 1.f, 0.7f), nullptr);
	m_skillLevelUpPanel->SetLayerIndex(LAYER_UI);

	shared_ptr<Material> cloneMaterial_skillLevelUpBtn = RESOURCES->Get<Material>(L"Btn_LevelUp_MouseOver");
	for (int i = 0; i < 4; i++)
	{
		auto skillLevelUpBtn = skillPanel->AddButton(Vec2(10 + 43 * i, 25), Vec2(48, 52), cloneMaterial_skillLevelUpBtn->Clone(), L"SkillLevelUpBtn" + to_wstring(i));
		skillLevelUpBtn->OnClick += [this, i] {
			m_player->GetSkill(i)->SkillLevelUp();
			m_player->GetStatus().availableSkillPoints--;
			};
	}*/


	CURSCENE->AddUIObject(m_panel, true);
	CURSCENE->RegisterUIParent(m_panel);

	//CURSCENE->AddUIObject(m_skillLevelUpPanel, true);
	//CURSCENE->RegisterUIParent(m_skillLevelUpPanel);
}

void GameHUDPanelUI::RegisterUIObject(shared_ptr<GameObject> uiObject)
{ 

}

void GameHUDPanelUI::UpdateSkillCoolDown()
{
	vector<shared_ptr<D2DText>> skillCoolDownTextUI;
	static vector<wstring> skillNames = { L"QSkillCoolDown", L"WSkillCoolDown", L"ESkillCoolDown", L"RSkillCoolDown" };

	for (const auto& skillName : skillNames) {
		skillCoolDownTextUI.push_back(m_panel->GetUIPanel()->GetD2DText(skillName));
	}

	for (int i = 0; i < 4; i++) {
		ISkill* skill = m_player->GetSkill(i);
		int skillCurCoolDown = (int)(skill->GetCurrentCooldown());

		if (skillCoolDownTextUI[i]) {
			skillCoolDownTextUI[i]->SetText(to_wstring(skillCurCoolDown));
			//skillCoolDownTextUI[i]->SetVisible(skillCurCoolDown > 0); // 0이면 숨김, 아니면 표시
		}
	}
}

void GameHUDPanelUI::UpdateStatBar()
{
	PlayerStatus& playerStatus = m_player->GetStatus();

	auto hpPanel = m_panel->GetUIPanel()->GetChildUIPanel(L"ChildHPPanel");
	auto spPanel = m_panel->GetUIPanel()->GetChildUIPanel(L"ChildSPPanel");
	auto expPanel = m_panel->GetUIPanel()->GetChildUIPanel(L"ChildEXPPanel");


	auto hpPanelText = hpPanel->GetD2DText(L"HPText");
	wstring hpText = to_wstring(playerStatus.hp) + L"/" + to_wstring(playerStatus.max_HP);
	hpPanelText->SetText(hpText);

	// 직접 size 수정 대신 SetLayerSize() 사용
	auto hpImageUI = hpPanel->GetImageUI(L"HPPanelImageUI");
	float ratio = ((float)playerStatus.hp / (float)playerStatus.max_HP);
	Vec2 newSize = Vec2(153.f * ratio, 10.f);
	Vec2 newPos = Vec2(153.f / 2.f - (153.f / 2.f) * (1 - ratio), 10.f / 2.f);
	hpImageUI->SetLayerSize(0, newSize);  // 레이어 0의 크기 변경
	hpImageUI->SetLayerPosition(0, newPos);


	auto spPanelText = spPanel->GetD2DText(L"SPText");
	wstring spText = to_wstring(playerStatus.stamina) + L"/" + to_wstring(playerStatus.max_Stamina);
	spPanelText->SetText(spText);

	// 직접 size 수정 대신 SetLayerSize() 사용
	auto spImageUI = spPanel->GetImageUI(L"SPPanelImageUI");
	ratio = ((float)playerStatus.stamina / (float)playerStatus.max_Stamina);
	newSize = Vec2(153.f * ratio, 10.f);
	newPos = Vec2(153.f / 2.f - (153.f / 2.f) * (1 - ratio), 10.f / 2.f);
	spImageUI->SetLayerSize(0, newSize);  // 레이어 0의 크기 변경
	spImageUI->SetLayerPosition(0, newPos);


	auto expPanelText = expPanel->GetD2DText(L"EXPText");
	wstring expText = to_wstring(playerStatus.curExp) + L"/" + to_wstring(playerStatus.curExpLimit);
	expPanelText->SetText(expText);

	// 직접 size 수정 대신 SetLayerSize() 사용
	auto expImageUI = expPanel->GetImageUI(L"EXPPanelImageUI");
	ratio = ((float)playerStatus.curExp / (float)playerStatus.curExpLimit);
	newSize = Vec2(153.f * ratio, 10.f);
	newPos = Vec2(153.f / 2.f - (153.f / 2.f) * (1 - ratio), 10.f / 2.f);
	expImageUI->SetLayerSize(0, newSize);  // 레이어 0의 크기 변경
	expImageUI->SetLayerPosition(0, newPos);
}

void GameHUDPanelUI::UpdatePlayerLevel()
{
	auto levelPanel = m_panel->GetUIPanel()->GetChildUIPanel(L"CharImagePanel")->GetChildUIPanel(L"LevelPanel");

	PlayerStatus& playerStatus = m_player->GetStatus();

	levelPanel->GetD2DText(L"LevelText")->SetText(to_wstring(playerStatus.level));
}