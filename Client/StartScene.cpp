#include "pch.h"
#include "StartScene.h"
#include "FogOfWar.h"
#include "CameraScript.h"
#include "UIPanel.h"
#include "Cursor.h"
#include "Graphics.h"
#include "Viewport.h"
#include "Camera.h"
#include "Material.h"
#include "Cursor.h"
#include "CharacterSelectScene.h"

void StartScene::Start()
{	
	m_defaultshader = make_shared<Shader>(L"FOW.fx");
	m_imageShader = make_shared<Shader>(L"ImageShader.fx");

	LoadStartSceneImages();

	CreateMainCamera();
	CreateUICamera();
	CreateLight();
	

	//CreateTestPanel();
	CreateLobbyBackGround();
	CreateSoundPanel();
	CreateCursor();


	SOUND->PlayBGM(L"BGM_Eternal world.wav", 0.5f);
	Scene::Start();
}

void StartScene::Update()
{
	Scene::Update();
}

void StartScene::FixedUpdate()
{
	Scene::FixedUpdate();
}

void StartScene::LateUpdate()
{
	Scene::LateUpdate();
}

void StartScene::Render()
{
	Scene::Render();
}

void StartScene::CreateMainCamera()
{
	// Camera
	auto camera = make_shared<GameObject>();
	//camera->GetTransform()->SetPosition(Vec3(0.f, 15.f, 15.f));
	camera->GetTransform()->SetPosition(Vec3{ 0.f, 30.f, -5.f });
	camera->AddComponent(make_shared<Camera>());
	camera->AddComponent(make_shared<CameraScript>());

	camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
	//CURSCENE->Add(camera);
	Add(camera);
}

void StartScene::CreateUICamera()
{
	// UICamera
	auto camera = make_shared<GameObject>();
	camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	camera->AddComponent(make_shared<Camera>());
	camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
	camera->GetCamera()->SetNear(0.1f);
	camera->GetCamera()->SetFar(100.0f);
	camera->GetCamera()->SetCullingMaskAll();
	camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, false);
	//CURSCENE->Add(camera);
	Add(camera);
}

void StartScene::CreateLight()
{
	// Light
	auto light = make_shared<GameObject>();
	light->AddComponent(make_shared<Light>());

	LightDesc lightDesc;
	lightDesc.ambient = Vec4(0.4f);
	lightDesc.diffuse = Vec4(1.f);
	lightDesc.specular = Vec4(0.1f);
	lightDesc.direction = Vec3(1.f, 1.f, 1.f);
	//light->GetLight()->SetLightDesc(lightDesc);
	light->GetTransform()->SetRotation(lightDesc.direction);
	light->GetTransform()->SetPosition(Vec3(0.f, 150.f, 0.f));
	static_pointer_cast<Light>(light->GetFixedComponent(ComponentType::Light))->SetLightDesc(lightDesc);
	//CURSCENE->Add(light);
	Add(light);
}

void StartScene::CreateLobbyBackGround()
{
	float width = GRAPHICS->GetViewport().GetWidth();
	float height = GRAPHICS->GetViewport().GetHeight();

	m_backPanel = make_shared<GameObject>();
	m_backPanel->SetName(L"UI_Panel");

	// UIPanel 컴포넌트 추가
	m_backPanel->AddComponent(make_shared<UIPanel>());
	
	m_backPanel->GetUIPanel()
		->Create(
			Vec2(width / 2.f,  height / 2.f),
			Vec2(width, height),
			Vec4(0.f),
			RESOURCES->Get<Material>(L"LobbyImage")
			//nullptr
		);
	
	// Panel Z값 명시적 설정
	//m_backPanel->GetTransform()->SetPosition(Vec3(0, 0, 0.9f));  // 가장 뒤쪽
	// 버튼 생성 - Normal과 Hover Material 준비
	shared_ptr<Material> normalMaterial = RESOURCES->Get<Material>(L"NormalBtnRollOver")->Clone();
	shared_ptr<Material> hoverMaterial = RESOURCES->Get<Material>(L"StartBtnRollOver")->Clone();
	// 패널에 버튼 추가
	auto button = m_backPanel->GetUIPanel()->AddButton(
		Vec2(185.f, 197.f),
		Vec2(162, 48),
		normalMaterial,  // 기본 Material을 Normal로 설정
		L"StartButton"
	);

	// 상태별 Material 설정
	button->SetNormalMaterial(normalMaterial);
	button->SetHoveredMaterial(hoverMaterial);
	button->SetPressedMaterial(hoverMaterial);

	//// 기존 클릭 이벤트 유지
	//button->AddOnClickedEvent([this]() {
	//	OnStartButtonClicked();
	//	});

	// Delegate 이벤트도 추가 
	button->OnClick += [this]() {
		//std::cout << "Delegate: Start Button Clicked!" << std::endl;
		OnStartButtonClicked();
		};

	button->OnHoverEnter += [this]() {
		std::cout << "Start Button Hovered!" << std::endl;
		OnButtonHover();
	};

	//button->OnHoverExit += []() {
	//	//std::cout << "Start Button Hover Exit!" << std::endl;
	//	};


	m_backPanel->GetUIPanel()->AddText(
		Vec2(175.f, 190.f),
		L"게임 시작",
		22.f,
		Color(1.f, 1.f, 1.f, 1.f),
		1.f,
		Color(0.f, 0.f, 0.f, 0.f),
		0.0f,
		L"GameStartText"
	);

	// 패널에 ImageUI 추가
	auto imageUI = m_backPanel->GetUIPanel()->AddImageUI(Vec2(0,0), L"MainImageUI");
	// **각 패널마다 Material을 복제하여 사용**

	// ImageUI에 이미지 레이어들 추가
	imageUI->AddImageLayer(0, Vec2(62, 130), Vec2(124, 42),
		RESOURCES->Get<Material>(L"StartBtnDeco_1"), 1);

	// ImageUI에 이미지 레이어들 추가
	imageUI->AddImageLayer(1, Vec2(42, 190), Vec2(169, 169),
		RESOURCES->Get<Material>(L"StartBtnDeco_3"), 1);

	// ImageUI에 이미지 레이어들 추가
	imageUI->AddImageLayer(2, Vec2(42, 190), Vec2(82, 93),
		RESOURCES->Get<Material>(L"StartBtnDeco_4"), 1);

	// ImageUI에 이미지 레이어들 추가
	imageUI->AddImageLayer(3, Vec2(220, 200), Vec2(441, 59),
		RESOURCES->Get<Material>(L"StartBtnDeco_2"), 1);


	//사운드 버튼 추가. 
	{
		// 패널에 버튼 추가
		auto soundbutton = m_backPanel->GetUIPanel()->AddButton(
			Vec2(185.f, 267.f),
			Vec2(162, 48),
			normalMaterial,  // 기본 Material을 Normal로 설정
			L"SoundButton"
		);

		// 상태별 Material 설정
		soundbutton->SetNormalMaterial(normalMaterial->Clone());
		soundbutton->SetHoveredMaterial(hoverMaterial->Clone());
		soundbutton->SetPressedMaterial(hoverMaterial->Clone());

		// Delegate 이벤트도 추가 
		soundbutton->OnClick += [this]() {
			//std::cout << "Delegate: Start Button Clicked!" << std::endl;
			//OnStartButtonClicked();
			OnSoundButtonClicked();
			EnableSoundPanel();
			};

		soundbutton->OnHoverEnter += [this]() {
			std::cout << "Start Button Hovered!" << std::endl;
			OnButtonHover();
		};

		auto imageSoundUI = m_backPanel->GetUIPanel()->AddImageUI(Vec2(0, 0), L"MainImageUI");
		// ImageUI에 이미지 레이어들 추가
		imageSoundUI->AddImageLayer(0, Vec2(62, 200), Vec2(124, 42),
			RESOURCES->Get<Material>(L"StartBtnDeco_1")->Clone(), 1);

		// ImageUI에 이미지 레이어들 추가
		imageSoundUI->AddImageLayer(1, Vec2(42, 260), Vec2(169, 169),
			RESOURCES->Get<Material>(L"StartBtnDeco_3")->Clone(), 1);

		// ImageUI에 이미지 레이어들 추가
		imageSoundUI->AddImageLayer(2, Vec2(42, 260), Vec2(82, 93),
			RESOURCES->Get<Material>(L"StartBtnDeco_4")->Clone(), 1);

		// ImageUI에 이미지 레이어들 추가
		imageSoundUI->AddImageLayer(3, Vec2(220, 270), Vec2(441, 59),
			RESOURCES->Get<Material>(L"StartBtnDeco_2")->Clone(), 1);



		m_backPanel->GetUIPanel()->AddText(
			Vec2(175.f, 260.f),
			L"사운드 설정",
			22.f,
			Color(1.f, 1.f, 1.f, 1.f),
			1.f,
			Color(0.f, 0.f, 0.f, 0.f),
			0.0f,
			L"GameSoundText"
		);

	}


	m_backPanel->SetLayerIndex(LAYER_UI);
	//CURSCENE->AddUIObject(m_backPanel, true);  // true = 부모
	//CURSCENE->RegisterUIParent(m_backPanel);

	AddUIObject(m_backPanel, true);  // CURSCENE 대신 AddUIObject 사용
	RegisterUIParent(m_backPanel);   // CURSCENE 대신 RegisterUIParent 사용
}

void StartScene::CreateSoundPanel()
{
	float width = GRAPHICS->GetViewport().GetWidth();
	float height = GRAPHICS->GetViewport().GetHeight();


	m_soundBackPanel = make_shared<GameObject>();
	m_soundBackPanel->SetName(L"Sound_BackPanel");
	m_soundBackPanel->AddComponent(make_shared<UIPanel>());
	m_soundBackPanel->GetUIPanel()
		->Create(
			Vec2(width / 2.f, height / 2.f),
			Vec2(width, height),
			Vec4(0.f),
			RESOURCES->Get<Material>(L"default")
			//nullptr
		);
	m_soundBackPanel->SetLayerIndex(LAYER_UI);

	AddUIObject(m_soundBackPanel, true);  // CURSCENE 대신 AddUIObject 사용
	RegisterUIParent(m_soundBackPanel);   // CURSCENE 대신 RegisterUIParent 사용

	m_soundPanel = make_shared<GameObject>();
	m_soundPanel->SetName(L"Sound_Panel");

	// UIPanel 컴포넌트 추가
	m_soundPanel->AddComponent(make_shared<UIPanel>());
	m_soundPanel->GetUIPanel()
		->Create(
			Vec2(width / 2.f, height / 2.f),
			Vec2(width / 2.f, height / 2.f),
			Vec4(1.f, 1.f, 1.f, 1.f),
			RESOURCES->Get<Material>(L"GrayImage")
			//nullptr
		);

	m_soundPanel->SetLayerIndex(LAYER_UI);

	auto backgroundBar = RESOURCES->Get<Material>(L"DarkBar");
	auto greenBar = RESOURCES->Get<Material>(L"GreenBar");
	
	m_soundPanel->GetUIPanel()->AddD2DText(Vec2(70, 100), L"BGM 음량", 24.0f,
		Vec4(1, 1, 1, 1), 1.0f, Vec4(0, 0, 0, 1), 2.f,
		L"BGM_Volume_Text", TextAlignment::Left);


	m_BGMSlider = m_soundPanel->GetUIPanel()->AddSliderUI(Vec2(400, 100), Vec2(500, 30), backgroundBar->Clone(), greenBar->Clone(), greenBar->Clone(), 0.f, 1.f, L"BGMSlider");
	m_BGMSlider->OnValueChanged += [this](float) {
		OnBGMSliderMove();
		};

	m_soundPanel->GetUIPanel()->AddD2DText(Vec2(70, 200), L"SFX 음량", 24.0f,
		Vec4(1, 1, 1, 1), 1.0f, Vec4(0, 0, 0, 1), 2.f,
		L"SFX_Volume_Text", TextAlignment::Left);

	m_SFXSlider = m_soundPanel->GetUIPanel()->AddSliderUI(Vec2(400, 200), Vec2(500, 30), backgroundBar->Clone(), greenBar->Clone(), greenBar->Clone(), 0.f, 1.f, L"SFXSlider");
	m_SFXSlider->OnValueChanged += [this](float) {
		OnSFXSliderMove();
		};
	
	m_soundPanel->GetUIPanel()->AddButton(Vec2(500, 250), Vec2(144, 61), backgroundBar->Clone());

	// 버튼용 Material 준비
	shared_ptr<Material> normalMaterial = RESOURCES->Get<Material>(L"CSScene_BtnDisabled")->Clone();
	shared_ptr<Material> hoverMaterial = RESOURCES->Get<Material>(L"CSScene_BtnPressed")->Clone();
	shared_ptr<Material> pressedMaterial = RESOURCES->Get<Material>(L"CSScene_BtnPressed")->Clone();


	auto DisableSoundPanelBtn = m_soundPanel->GetUIPanel()->AddButton(Vec2(500, 250), Vec2(144, 61), normalMaterial, L"DisableButton");

	DisableSoundPanelBtn->SetNormalMaterial(normalMaterial);
	DisableSoundPanelBtn->SetHoveredMaterial(hoverMaterial);
	DisableSoundPanelBtn->SetPressedMaterial(pressedMaterial);

	DisableSoundPanelBtn->OnClick += [this]() {
		SOUND->PlaySound(L"SFX/oui_Banner_Click.wav", 2, 0.5f);
		DisableSoundPanel();
		};

	DisableSoundPanelBtn->OnHoverEnter += [this]() {
		OnButtonHover();
		};
	m_soundPanel->GetUIPanel()->SetVisible(false);

	AddUIObject(m_soundPanel, true);  // CURSCENE 대신 AddUIObject 사용
	RegisterUIParent(m_soundPanel);   // CURSCENE 대신 RegisterUIParent 사용
}

void StartScene::CreateCursor()
{
	auto cursorObj = make_shared<GameObject>();
	cursorObj->SetName(L"MouseCursorObject");

	m_cursor = make_shared<Cursor>();
	cursorObj->AddComponent(m_cursor);
	m_cursor->SetVisible(true);

	CURSCENE->Add(cursorObj);
}



void StartScene::LoadStartSceneImages()
{
	LoadBtnImages();
	LoadLobbyImages();
	LoadSliderImages();
}

void StartScene::LoadBtnImages()
{
	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(m_imageShader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	shared_ptr<Material> startBtnRollOver = make_shared<Material>();
	SetupUIMaterial(startBtnRollOver);
	auto StartBtnRollOverTexture = RESOURCES->Load<Texture>(L"StartBtnRollOver", L"..\\Resources\\Textures\\UI_Btn\\StartBtn\\Img_LobbyTabBtn_Bg_Select_01.png");
	startBtnRollOver->SetDiffuseMap(StartBtnRollOverTexture);
	MaterialDesc& startBtnRollOverDesc = startBtnRollOver->GetMaterialDesc();
	startBtnRollOverDesc.ambient = Vec4(1.f);
	startBtnRollOverDesc.diffuse = Vec4(1.f);
	startBtnRollOverDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"StartBtnRollOver", startBtnRollOver);

	shared_ptr<Material> normalBtnRollOver = make_shared<Material>();
	SetupUIMaterial(normalBtnRollOver);
	auto NormalBtnRollOverTexture = RESOURCES->Load<Texture>(L"NormalBtnRollOver", L"..\\Resources\\Textures\\UI_Btn\\StartBtn\\Img_LobbyTabBtn_Bg_Select_02.png");
	normalBtnRollOver->SetDiffuseMap(NormalBtnRollOverTexture);
	MaterialDesc& normalBtnRollOverDesc = normalBtnRollOver->GetMaterialDesc();
	normalBtnRollOverDesc.ambient = Vec4(1.f);
	normalBtnRollOverDesc.diffuse = Vec4(1.f);
	normalBtnRollOverDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"NormalBtnRollOver", normalBtnRollOver);

	shared_ptr<Material> startBtnDeco1 = make_shared<Material>();
	SetupUIMaterial(startBtnDeco1);
	auto startBtnDeco_1 = RESOURCES->Load<Texture>(L"StartBtnDeco_1", L"..\\Resources\\Textures\\UI_Btn\\StartBtn\\Img_LobbyGameStartBtn_Deco01.png");
	startBtnDeco1->SetDiffuseMap(startBtnDeco_1);
	MaterialDesc& startBtnDeco1Desc = startBtnDeco1->GetMaterialDesc();
	startBtnDeco1Desc.ambient = Vec4(1.f);
	startBtnDeco1Desc.diffuse = Vec4(1.f);
	startBtnDeco1Desc.specular = Vec4(1.0f);
	RESOURCES->Add(L"StartBtnDeco_1", startBtnDeco1);

	shared_ptr<Material> startBtnDeco2 = make_shared<Material>();
	SetupUIMaterial(startBtnDeco2);
	auto startBtnDeco_2 = RESOURCES->Load<Texture>(L"StartBtnDeco_2", L"..\\Resources\\Textures\\UI_Btn\\StartBtn\\Img_LobbyGameStartBtn_Deco02.png");
	startBtnDeco2->SetDiffuseMap(startBtnDeco_2);
	MaterialDesc& startBtnDeco2Desc = startBtnDeco2->GetMaterialDesc();
	startBtnDeco2Desc.ambient = Vec4(1.f);
	startBtnDeco2Desc.diffuse = Vec4(1.f);
	startBtnDeco2Desc.specular = Vec4(1.0f);
	RESOURCES->Add(L"StartBtnDeco_2", startBtnDeco2);

	shared_ptr<Material> startBtnDeco3 = make_shared<Material>();
	SetupUIMaterial(startBtnDeco3);
	auto startBtnDeco_3 = RESOURCES->Load<Texture>(L"StartBtnDeco_3", L"..\\Resources\\Textures\\UI_Btn\\StartBtn\\Img_LobbyGameStartBtn_Deco03.png");
	startBtnDeco3->SetDiffuseMap(startBtnDeco_3);
	MaterialDesc& startBtnDeco3Desc = startBtnDeco3->GetMaterialDesc();
	startBtnDeco3Desc.ambient = Vec4(1.f);
	startBtnDeco3Desc.diffuse = Vec4(1.f);
	startBtnDeco3Desc.specular = Vec4(1.0f);
	RESOURCES->Add(L"StartBtnDeco_3", startBtnDeco3);

	shared_ptr<Material> startBtnDeco4 = make_shared<Material>();
	SetupUIMaterial(startBtnDeco4);
	auto startBtnDeco_4 = RESOURCES->Load<Texture>(L"StartBtnDeco_4", L"..\\Resources\\Textures\\UI_Btn\\StartBtn\\Img_LobbyGameStartBtn_Deco04.png");
	startBtnDeco4->SetDiffuseMap(startBtnDeco_4);
	MaterialDesc& startBtnDeco4Desc = startBtnDeco4->GetMaterialDesc();
	startBtnDeco4Desc.ambient = Vec4(1.f);
	startBtnDeco4Desc.diffuse = Vec4(1.f);
	startBtnDeco4Desc.specular = Vec4(1.0f);
	RESOURCES->Add(L"StartBtnDeco_4", startBtnDeco4);

	shared_ptr<Material> CSScene_BtnPressedMat = make_shared<Material>();
	SetupUIMaterial(CSScene_BtnPressedMat);
	auto CSScene_BtnPressedMatTexture = RESOURCES->Load<Texture>(L"CSScene_BtnPressed", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\Btn_MatchingStart_01.png");
	CSScene_BtnPressedMat->SetDiffuseMap(CSScene_BtnPressedMatTexture);
	MaterialDesc& CSScene_BtnPressedMatDesc = CSScene_BtnPressedMat->GetMaterialDesc();
	CSScene_BtnPressedMatDesc.ambient = Vec4(1.f);
	CSScene_BtnPressedMatDesc.diffuse = Vec4(1.f);
	CSScene_BtnPressedMatDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CSScene_BtnPressed", CSScene_BtnPressedMat);

	shared_ptr<Material> CSScene_BtnDisabledMat = make_shared<Material>();
	SetupUIMaterial(CSScene_BtnDisabledMat);
	auto CSScene_BtnDisabledMatTexture = RESOURCES->Load<Texture>(L"CSScene_BtnDisabled", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\Btn_MatchingStart_Disabled_01.png");
	CSScene_BtnDisabledMat->SetDiffuseMap(CSScene_BtnDisabledMatTexture);
	MaterialDesc& CSScene_BtnDisabledMatDesc = CSScene_BtnDisabledMat->GetMaterialDesc();
	CSScene_BtnDisabledMatDesc.ambient = Vec4(1.f);
	CSScene_BtnDisabledMatDesc.diffuse = Vec4(1.f);
	CSScene_BtnDisabledMatDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CSScene_BtnDisabled", CSScene_BtnDisabledMat);
}

void StartScene::LoadSliderImages()
{
	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(m_imageShader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
	};

	shared_ptr<Material> barBackground = make_shared<Material>();
	SetupUIMaterial(barBackground);
	auto barBackground_tex = RESOURCES->Load<Texture>(L"DarkBar", L"..\\Resources\\Textures\\UI\\status\\EmptyBar_UI.png");
	barBackground->SetDiffuseMap(barBackground_tex);
	MaterialDesc& barBackgroundDesc = barBackground->GetMaterialDesc();
	barBackgroundDesc.ambient = Vec4(1.f);
	barBackgroundDesc.diffuse = Vec4(1.f);
	barBackgroundDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"DarkBar", barBackground);

	shared_ptr<Material> barGreen = make_shared<Material>();
	SetupUIMaterial(barGreen);
	auto bargreen_tex = RESOURCES->Load<Texture>(L"GreenBar", L"..\\Resources\\Textures\\UI\\StatusBar\\Gauge\\Img_Main_Gage_01.png");
	barGreen->SetDiffuseMap(bargreen_tex);
	MaterialDesc& barGreenDesc = barGreen->GetMaterialDesc();
	barGreenDesc.ambient = Vec4(1.f);
	barGreenDesc.diffuse = Vec4(1.f);
	barGreenDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"GreenBar", barGreen);
}

void StartScene::LoadLobbyImages()
{
	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(m_imageShader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	shared_ptr<Material> backGround = make_shared<Material>();
	SetupUIMaterial(backGround);
	auto texture = RESOURCES->Load<Texture>(L"LobbyImage", L"..\\Resources\\Textures\\UI\\LobbyImage.png");
	backGround->SetDiffuseMap(texture);
	MaterialDesc& backGroundDesc = backGround->GetMaterialDesc();
	backGroundDesc.ambient = Vec4(1.f);
	backGroundDesc.diffuse = Vec4(1.f);
	backGroundDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"LobbyImage", backGround);


	shared_ptr<Material> SoundGround = make_shared<Material>();
	SetupUIMaterial(SoundGround);
	auto grayTexture = RESOURCES->Load<Texture>(L"GrayBackGround", L"..\\Resources\\Textures\\UI\\time\\Nightshade2.png");
	SoundGround->SetDiffuseMap(grayTexture);
	MaterialDesc& SoundGroundDesc = SoundGround->GetMaterialDesc();
	SoundGroundDesc.ambient = Vec4(1.f);
	SoundGroundDesc.diffuse = Vec4(1.f);
	SoundGroundDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"GrayImage", SoundGround);
}


void StartScene::OnStartButtonClicked()
{
	std::wcout << L"Start Button Clicked! Changing to Character Select Scene...\n";
	SOUND->PlaySound(L"SFX/oui_matchClick2.wav", 2, 0.5f);

	auto characterSelectScene = make_shared<CharacterSelectScene>();
	SCENE->ChangeScene(characterSelectScene);
}

void StartScene::OnSoundButtonClicked()
{
	SOUND->PlaySound(L"SFX/oui_Banner_Click.wav", 2, 0.5f);
}

void StartScene::EnableSoundPanel()
{
	m_soundPanel->GetUIPanel()->SetVisible(true);
	cout << "SoundPanel Enable\n";
}

void StartScene::DisableSoundPanel()
{
	m_soundPanel->GetUIPanel()->SetVisible(false);
	cout << "SoundPanel Disable\n";
}

void StartScene::OnBGMSliderMove()
{
	//cout << m_BGMSlider->GetValue() << "\n";
	SOUND->SetBGMVolume(m_BGMSlider->GetValue());
}

void StartScene::OnSFXSliderMove()
{
	SOUND->SetSFXVolume(m_SFXSlider->GetValue());
}

void StartScene::OnButtonHover()
{
	SOUND->PlaySound(L"SFX/oui_mainMenu_hover.wav", 2, 0.5f);

}
