#include "pch.h"
#include "StartScene.h"
#include "FogOfWar.h"
#include "CameraScript.h"
#include "UIPanel.h"
#include "Graphics.h"
#include "Viewport.h"
#include "Camera.h"
#include "Material.h"
#include "CharacterSelectScene.h"

void StartScene::Start()
{	
	

	m_defaultshader = make_shared<Shader>(L"FOW.fx");
	m_imageShader = make_shared<Shader>(L"ImageShader.fx");

	LoadStartSceneImages();

	CreateMainCamera();
	CreateUICamera();
	//Default Light
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
	// 디퍼드 렌더링 비활성화
	//RENDER->SetDeferredRendering(false);

	//CreateTestPanel();
	CreateLobbyBackGround();

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
			RESOURCES->Get<Material>(L"LobbyImage")
			//nullptr
		);
	// Panel Z값 명시적 설정
	m_backPanel->GetTransform()->SetPosition(Vec3(0, 0, 0.9f));  // 가장 뒤쪽
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

	// 기존 클릭 이벤트 유지
	button->AddOnClickedEvent([this]() {
		OnStartButtonClicked();
		});

	// Delegate 이벤트도 추가 (선택사항)
	button->OnClick += [this]() {
		std::wcout << L"Delegate: Start Button Clicked!" << std::endl;
		};

	button->OnHoverEnter += []() {
		std::wcout << L"Start Button Hovered!" << std::endl;
		};

	button->OnHoverExit += []() {
		std::wcout << L"Start Button Hover Exit!" << std::endl;
		};


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


	m_backPanel->SetLayerIndex(LAYER_UI);
	//CURSCENE->AddUIObject(m_backPanel, true);  // true = 부모
	//CURSCENE->RegisterUIParent(m_backPanel);

	AddUIObject(m_backPanel, true);  // CURSCENE 대신 AddUIObject 사용
	RegisterUIParent(m_backPanel);   // CURSCENE 대신 RegisterUIParent 사용
}



void StartScene::LoadStartSceneImages()
{
	LoadBtnImages();
	LoadLobbyImages();
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
}


void StartScene::OnStartButtonClicked()
{
	std::wcout << L"Start Button Clicked! Changing to Character Select Scene...\n";

	auto characterSelectScene = make_shared<CharacterSelectScene>();
	SCENE->ChangeScene(characterSelectScene);
}