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

const vector<wstring> characterNames = {
		L"Bianca",
		L"Nicky",
		L"Abigail",
		L"Aiden",
		L"Chiara",
		L"Daniel",
		L"Darko",
		L"DebiMarlene",
		L"Eva",
		L"Garnet",
		L"Isol",
		L"Laura",
		L"Nadine",
		L"Nathapon",
		L"Niah",
		L"Silvia",
		L"Sua",
		L"Tia",
		L"Yuki"
};

const vector<wstring> characterKoreanNames = {
		L"비앙카",
		L"니키",
		L"아비게일",
		L"에이든",
		L"키아라",
		L"다니엘",
		L"다르코",
		L"데비마를렌",
		L"이바",
		L"가넷",
		L"아이솔",
		L"라우라",
		L"나딘",
		L"나타폰",
		L"니아",
		L"실비아",
		L"수아",
		L"띠아",
		L"유키"
};

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
	LoadCharacterSlotImages();
	LoadCharacterImages();
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
	
	vector<wstring> slotNames = {
		L"CharSlotNormal",
		L"CharRandomSlotNormal",
		L"CharRandomSlotRollOver",
		L"CharSlotRollOver",
	};
	vector<wstring> fileNames = {
		L"Img_Slot_Character_Route.png",
		L"Img_Slot_CharacterList_Random.png",
		L"Img_Slot_CharacterList_Random_Over.png",
		L"Img_Slot_CharacterList_Select.png"
	};


	wstring prefixPath = L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\";
	for (int i = 0; i < slotNames.size(); i++)
	{
		shared_ptr<Material> slot = make_shared<Material>();
		SetupUIMaterial(slot);

		wstring finalPath = prefixPath + fileNames[i];

		auto slotTexture = RESOURCES->Load<Texture>(slotNames[i], finalPath);
		
		slot->SetDiffuseMap(slotTexture);
		MaterialDesc& slotDesc = slot->GetMaterialDesc();
		slotDesc.ambient = Vec4(1.f);
		slotDesc.diffuse = Vec4(1.f);
		slotDesc.specular = Vec4(1.0f);
		RESOURCES->Add(slotNames[i], slot);
	}


	shared_ptr<Material> charRandomImage = make_shared<Material>();
	SetupUIMaterial(charRandomImage);
	auto charRandomImageTexture = RESOURCES->Load<Texture>(L"CharLobbyRandom", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\CharLobby_Random.png");
	charRandomImage->SetDiffuseMap(charRandomImageTexture);
	MaterialDesc& charRandomImageDesc = charRandomImage->GetMaterialDesc();
	charRandomImageDesc.ambient = Vec4(1.f);
	charRandomImageDesc.diffuse = Vec4(1.f);
	charRandomImageDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CharLobbyRandom", charRandomImage);
}

void CharacterSelectScene::LoadCharacterImages()
{
	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(m_imageShader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	
	wstring prefixTag = L"CharLobby";
	wstring prefixPath = L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\CharacterImages\\";

	for (int i = 0; i < characterNames.size(); i++)
	{
		shared_ptr<Material> charLobbyImage = make_shared<Material>();
		SetupUIMaterial(charLobbyImage);

		wstring tag = prefixTag + characterNames[i];
		wstring path = prefixPath + characterNames[i] + L"\\" + prefixTag + L"_" + characterNames[i] + L"_S000.png";
		auto charLobbyTexture = RESOURCES->Load<Texture>(tag, path);
		
		
		charLobbyImage->SetDiffuseMap(charLobbyTexture);
		MaterialDesc& charLobbyDesc = charLobbyImage->GetMaterialDesc();
		charLobbyDesc.ambient =		Vec4(1.f);
		charLobbyDesc.diffuse =		Vec4(1.f);
		charLobbyDesc.specular =	Vec4(1.0f);
		RESOURCES->Add(tag, charLobbyImage);
	}
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
	Vec2 scrollPos = Vec2(280, 340);
	Vec2 scrollSize = Vec2(500, 415);

	scrollView->Create(scrollPos, scrollSize, nullptr);
	scrollView->SetScrollDirection(ScrollDirection::Vertical);
	scrollView->SetContentSize(Vec2(500.0f, 1800.0f)); // 컨텐츠는 더 큰 크기
	scrollView->SetScrollSpeed(30.0f);
	// 픽셀 클리핑 활성화
	scrollView->SetPixelClipping(true);	


	int characterIndex = 0;

	for (int i = 0; i < 20; i++)
	{


		Vec2 cardPos = Vec2(-208 + (i%5) * 100, -140 + (i/5) * 122); // 세로로 배치
		Vec2 cardSize = Vec2(106, 166);

		shared_ptr<UIPanel> panel = scrollView->AddPanel(cardPos, cardSize, nullptr);

		if (i == 0)
		{
			shared_ptr<Material> uniqueMaterial = RESOURCES->Get<Material>(L"CharRandomSlotNormal")->Clone();
		
			// 패널에 버튼도 추가 가능
			auto button = panel->AddButton(Vec2(61, 79), Vec2(122, 158),
				uniqueMaterial, L"CharRandomSlotNormal");

			auto text = panel->AddText(
				Vec2(50.f, 120.f),
				L"Random",
				13.f,
				Vec4(1.f),
				1.f,
				Vec4(0.f),
				0.f,
				L"GameTest",
				TextAlignment::Left
			);

			shared_ptr<Material> uniqueMaterial2 = RESOURCES->Get<Material>(L"CharLobbyRandom")->Clone();
			auto imageUI = panel->AddImageUI(Vec2(0.f, 0.f));
			imageUI->AddImageLayer(1, Vec2(60, 78), Vec2(42, 64), uniqueMaterial2, 1);
		}
		else
		{
			// 각 패널마다 Material을 복제하여 사용
			shared_ptr<Material> uniqueMaterial = RESOURCES->Get<Material>(L"CharSlotNormal")->Clone();
			shared_ptr<Material> uniqueMaterial3 = RESOURCES->Get<Material>(L"CharSlotRollOver")->Clone();

			// 패널에 버튼도 추가 가능
			auto button = panel->AddButton(Vec2(61, 79), Vec2(122, 158),
				uniqueMaterial, L"CharSlotNormal");

			// 버튼 상태별 Material 설정
			button->SetNormalMaterial(uniqueMaterial);
			button->SetHoveredMaterial(uniqueMaterial3);
			button->SetPressedMaterial(uniqueMaterial3);

			// Delegate에 함수 등록 (함수 호출이 아님!)
			/*button->OnClick += []() {
				std::cout << "Button clicked!" << std::endl;
				};

			button->OnHoverEnter += []() {
				std::cout << "Mouse entered button!" << std::endl;
				};

			button->OnHoverExit += []() {
				std::cout << "Mouse exited button!" << std::endl;
				};*/

			// OnStateChanged에 함수 등록 - ButtonState 매개변수를 받는 함수
			/*button->OnStateChanged += [](ButtonState state) {
				switch (state) {
				case ButtonState::Normal:
					std::cout << "Button state: Normal" << std::endl;
					break;
				case ButtonState::Hovered:
					std::cout << "Button state: Hovered" << std::endl;
					break;
				case ButtonState::Pressed:
					std::cout << "Button state: Pressed" << std::endl;
					break;
				case ButtonState::Disabled:
					std::cout << "Button state: Disabled" << std::endl;
					break;
				}
				};*/

			auto text = panel->AddText(
				Vec2(50.f, 120.f),
				characterKoreanNames[characterIndex],
				13.f,
				Vec4(1.f),
				1.f,
				Vec4(0.f),
				0.f,
				L"GameTest",
				TextAlignment::Left
			);
			// 각 패널마다 Material을 복제하여 사용
			wstring charTag = L"CharLobby" + characterNames[characterIndex++];
			shared_ptr<Material> uniqueMaterial2 = RESOURCES->Get<Material>(charTag)->Clone();
			auto imageUI = panel->AddImageUI(Vec2(0.f, 0.f));
			imageUI->AddImageLayer(1, Vec2(60, 78), Vec2(121, 157), uniqueMaterial2, 1);
		}
		
	}

	AddUIObject(scrollViewObj, true);
	RegisterUIParent(scrollViewObj);
}