#include "pch.h"
#include "CharacterSelectScene.h"
#include "FogOfWar.h"
#include "CameraScript.h"

#include "UIPanel.h"

#include "Graphics.h"
#include "Viewport.h"

#include "Camera.h"

#include "Material.h"
#include "ScrollView.h"

void CharacterSelectScene::Start()
{	
	m_defaultshader = make_shared<Shader>(L"FOW.fx");
	m_imageShader = make_shared<Shader>(L"ImageShader.fx");

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

	LoadCharacterSelectSceneImages();
	LoadCharacterSlotImages();

	CreateBackGround();
	CreateScrollableCharacterList();
	Scene::Start();
}

void CharacterSelectScene::Update()
{
	Scene::Update();
}

void CharacterSelectScene::FixedUpdate()
{
	Scene::FixedUpdate();
}

void CharacterSelectScene::LateUpdate()
{
	Scene::LateUpdate();
}

void CharacterSelectScene::Render()
{
	Scene::Render();
}

void CharacterSelectScene::CreateMainCamera()
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

void CharacterSelectScene::CreateUICamera()
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

void CharacterSelectScene::CreateBackGround()
{
	float width = GRAPHICS->GetViewport().GetWidth();
	float height = GRAPHICS->GetViewport().GetHeight();

	m_backPanel = make_shared<GameObject>();
	m_backPanel->SetName(L"CSScene_BackGround");

	// UIPanel 컴포넌트 추가
	m_backPanel->AddComponent(make_shared<UIPanel>());
	m_backPanel->GetUIPanel()
		->Create(
			Vec2(width / 2.f, height / 2.f),
			Vec2(width, height),
			RESOURCES->Get<Material>(L"CSSceneBackGround")
		);
	// Panel Z값 명시적 설정
	m_backPanel->GetTransform()->SetPosition(Vec3(0, 0, 0.9f));  // 가장 뒤쪽

	// 패널에 ImageUI 추가
	auto imageUI = m_backPanel->GetUIPanel()->AddImageUI(Vec2(0, 0), L"MainImageUI");
	// ImageUI에 이미지 레이어들 추가
	imageUI->AddImageLayer(0, Vec2(974, 341), Vec2(490, 540),
		RESOURCES->Get<Material>(L"CSSceneBackGroundDeco_1"), 1);

	m_backPanel->SetLayerIndex(LAYER_UI);
	//CURSCENE->AddUIObject(m_backPanel, true);  // true = 부모
	//CURSCENE->RegisterUIParent(m_backPanel);

	AddUIObject(m_backPanel, true);
	RegisterUIParent(m_backPanel);
}


void CharacterSelectScene::LoadCharacterSelectSceneImages()
{
	LoadBackGround();
}

void CharacterSelectScene::LoadBackGround()
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
	auto backGroundTexture = RESOURCES->Load<Texture>(L"CSSceneBackGround", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\CharaterSelectSceneImage.png");
	backGround->SetDiffuseMap(backGroundTexture);
	MaterialDesc& backGroundDesc = backGround->GetMaterialDesc();
	backGroundDesc.ambient = Vec4(1.f);
	backGroundDesc.diffuse = Vec4(1.f);
	backGroundDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CSSceneBackGround", backGround);

	shared_ptr<Material> backGroundDeco1 = make_shared<Material>();
	SetupUIMaterial(backGroundDeco1);
	auto backGroundDeco1Texture = RESOURCES->Load<Texture>(L"CSSceneBackGroundDeco_1", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\Img_NonCharacterCard.png");
	backGroundDeco1->SetDiffuseMap(backGroundDeco1Texture);
	MaterialDesc& backGroundDeco1Desc = backGroundDeco1->GetMaterialDesc();
	backGroundDeco1Desc.ambient = Vec4(1.f);
	backGroundDeco1Desc.diffuse = Vec4(1.f);
	backGroundDeco1Desc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CSSceneBackGroundDeco_1", backGroundDeco1);
}

void CharacterSelectScene::LoadCharacterSlotImages()
{
	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(m_imageShader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	shared_ptr<Material> charSlotNormal = make_shared<Material>();
	SetupUIMaterial(charSlotNormal);
	auto charSlotNormalTexture = RESOURCES->Load<Texture>(L"CharSlotNormal", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\Img_Slot_Character_Route.png");
	charSlotNormal->SetDiffuseMap(charSlotNormalTexture);
	MaterialDesc& charSlotNormalDesc = charSlotNormal->GetMaterialDesc();
	charSlotNormalDesc.ambient = Vec4(1.f);
	charSlotNormalDesc.diffuse = Vec4(1.f);
	charSlotNormalDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CharSlotNormal", charSlotNormal);


	shared_ptr<Material> charLobbyNicky = make_shared<Material>();
	SetupUIMaterial(charLobbyNicky);
	auto charLobbyNickyTexture = RESOURCES->Load<Texture>(L"CharLobbyNicky", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\CharacterImages\\CharLobby_Nicky_S000.png");
	charLobbyNicky->SetDiffuseMap(charLobbyNickyTexture);
	MaterialDesc& charLobbyNickyDesc = charLobbyNicky->GetMaterialDesc();
	charLobbyNickyDesc.ambient = Vec4(1.f);
	charLobbyNickyDesc.diffuse = Vec4(1.f);
	charLobbyNickyDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CharLobbyNicky", charLobbyNicky);
}

void CharacterSelectScene::CreateScrollableCharacterList()
{
	// ScrollView 생성
	auto scrollViewObj = make_shared<GameObject>();
	scrollViewObj->SetName(L"CharacterScrollView");

	auto scrollView = make_shared<ScrollView>();
	scrollViewObj->AddComponent(scrollView);

	// 화면 중앙에 400x600 크기의 스크롤뷰 생성
	float width = GRAPHICS->GetViewport().GetWidth();
	float height = GRAPHICS->GetViewport().GetHeight();
	Vec2 scrollPos = Vec2(width / 2.0f, height / 2.0f);
	Vec2 scrollSize = Vec2(400.0f, 600.0f);

	scrollView->Create(scrollPos, scrollSize, nullptr);
	scrollView->SetScrollDirection(ScrollDirection::Vertical);
	scrollView->SetContentSize(Vec2(400.0f, 1800.0f)); // 컨텐츠는 더 큰 크기
	scrollView->SetScrollSpeed(30.0f);

	// 캐릭터 카드들 추가
	for (int i = 0; i < 3; i++) {
		Vec2 cardPos = Vec2(300, i * 150.0f); // 세로로 배치
		Vec2 cardSize = Vec2(350.0f, 100.0f);


		shared_ptr<UIPanel> panel = scrollView->AddPanel(cardPos, cardSize, nullptr);

		//// 패널에 버튼도 추가 가능
		//auto button = panel->AddButton(Vec2(0.f,0.f), Vec2(106, 166),
		//	RESOURCES->Get<Material>(L"CharSlotNormal"), L"CharSlotNormal");

		/*auto text = panel->AddText(
			Vec2(0.f,0.f),
			L"게임시작",
			20.f,
			Vec4(1.f),
			1.f,
			Vec4(0.f),
			0.f,
			L"GameTest"
		);*/

		// **각 패널마다 Material을 복제하여 사용**
		shared_ptr<Material> uniqueMaterial = RESOURCES->Get<Material>(L"CharSlotNormal")->Clone();

		auto imageUI = panel->AddImageUI(Vec2(0.f, 0.f));

		imageUI->AddImageLayer(1, Vec2(0.f, 0.f), Vec2(106, 166), uniqueMaterial, 1);

		//cout << "button pos : " << pos.x << " , " << pos.y << endl;

		//// 패널에 ImageUI 추가
		//auto imageUI = panel->GetUIPanel()->AddImageUI(Vec2(0, 0), L"MainImageUI");
		//// ImageUI에 이미지 레이어들 추가
		//imageUI->AddImageLayer(0, Vec2(0, 200), Vec2(106, 166),
		//	RESOURCES->Get<Material>(L"CharSlotNormal"), 1);

		/*auto characterPanel = scrollView->AddPanel(
			cardPos,
			cardSize,
			nullptr,
			L"CharacterCard_" + std::to_wstring(i)
		);*/

		/*auto imageObj = make_shared<GameObject>();

		auto imageComponent = make_shared<ImageUI>();

		imageObj->AddComponent(imageComponent);

		imageComponent->AddImageLayer(0, Vec2(0, 0), Vec2(106, 166),
			RESOURCES->Get<Material>(L"CharSlotNormal"), 1);

		imageComponent->AddImageLayer(1, Vec2(0, 0), Vec2(106, 166),
			RESOURCES->Get<Material>(L"CharLobbyNicky"), 1);*/

		//// 텍스트 GameObject 생성하여 AddUIElement로 추가
		//auto textObj = make_shared<GameObject>();
		//textObj->SetName(L"CharacterText_" + std::to_wstring(i));

		//// Text 컴포넌트 추가 및 설정
		//auto textComponent = make_shared<Text>();
		//textObj->AddComponent(textComponent);

		//Vec2 textPos = Vec2(-100, i * 120.0f); // 패널 내 상대 위치
		//textComponent->Create(Vec2(0, 0), L"Character " + std::to_wstring(i + 1), 20.0f, Vec4(1, 1, 1, 1));

		//// ScrollView에 텍스트 요소 추가 (중요: AddUIElement 사용)
		
	}

	AddUIObject(scrollViewObj, true);
	RegisterUIParent(scrollViewObj);
}