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

#include "Button.h"

const vector<int> skinCount = {
	5, 6, 3, 4, 5,
	4, 2, 4, 5, 2,
	5, 5, 5, 3, 1,
	5, 7, 4, 6
};

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
	CreateLight();
	
	LoadCharacterSelectSceneImages();

	CreateBackGround();
	CreateScrollableCharacterList();
	CreateScrollableSkinList();
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

void CharacterSelectScene::CreateLight()
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

void CharacterSelectScene::LoadCharacterSelectSceneImages()
{
	LoadCharacterListSlotImages();
	LoadCharacterImages();
	LoadCharacterSkinListSlotImages();
	LoadBackGround();
	LoadCharacterFullAndHalfImages();
}

void CharacterSelectScene::LoadBackGround()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
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

void CharacterSelectScene::LoadCharacterListSlotImages()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		}; 
	
	vector<wstring> slotNames = {
		L"CharSlotNormal",
		L"CharRandomSlotNormal",
		L"CharRandomSlotRollOver",
		L"CharSlotRollOver"
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

void CharacterSelectScene::LoadCharacterSkinListSlotImages()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};


	shared_ptr<Material> charSkinListSlot = make_shared<Material>();
	SetupUIMaterial(charSkinListSlot);
	auto charSkinListSlotTexture = RESOURCES->Load<Texture>(L"CharSkinSlotNormal", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\Img_SkinSlot_Basic_01_Common.png");
	charSkinListSlot->SetDiffuseMap(charSkinListSlotTexture);
	MaterialDesc& charSkinListSlotDesc = charSkinListSlot->GetMaterialDesc();
	charSkinListSlotDesc.ambient = Vec4(1.f);
	charSkinListSlotDesc.diffuse = Vec4(1.f);
	charSkinListSlotDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CharSkinSlotNormal", charSkinListSlot);

	shared_ptr<Material> charSkinListSlotRollOver = make_shared<Material>();
	SetupUIMaterial(charSkinListSlotRollOver);
	auto charSkinListSlotRollOverTexture = RESOURCES->Load<Texture>(L"CharSkinSlotRollOver", L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\Img_SkinSlot_Basic_01_RollOver.png");
	charSkinListSlotRollOver->SetDiffuseMap(charSkinListSlotRollOverTexture);
	MaterialDesc& charSkinListSlotRollOverDesc = charSkinListSlotRollOver->GetMaterialDesc();
	charSkinListSlotRollOverDesc.ambient = Vec4(1.f);
	charSkinListSlotRollOverDesc.diffuse = Vec4(1.f);
	charSkinListSlotRollOverDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"CharSkinSlotRollOver", charSkinListSlotRollOver);
}

void CharacterSelectScene::LoadCharacterImages()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");


	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
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
		charLobbyDesc.specular =	Vec4(1.f);
		RESOURCES->Add(tag, charLobbyImage);
	}
}

void CharacterSelectScene::LoadCharacterFullAndHalfImages()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");


	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	wstring prefixTagFull = L"Full";
	wstring prefixTagHalf = L"Half";
	wstring prefixPath = L"..\\Resources\\Textures\\UI\\CharacterSelectScene\\CharacterImages\\";

	wstring tag, path;

	for (int i = 0; i < characterNames.size(); i++)
	{
		for (int j = 0; j < skinCount[i]; j++)
		{
			////////////Full Image////////////////////////
			shared_ptr<Material> charFullImage = make_shared<Material>();
			SetupUIMaterial(charFullImage);

			tag = L"Char" + prefixTagFull + L"_" + characterNames[i] + L"_S00" + to_wstring(j);
			path = prefixPath + characterNames[i] + L"\\" + tag + L".png";
			auto charFullTexture = RESOURCES->Load<Texture>(tag, path);

			charFullImage->SetDiffuseMap(charFullTexture);
			MaterialDesc& charFullImageDesc = charFullImage->GetMaterialDesc();
			charFullImageDesc.ambient = Vec4(1.f);
			charFullImageDesc.diffuse = Vec4(1.f);
			charFullImageDesc.specular = Vec4(1.0f);
			RESOURCES->Add(tag, charFullImage);
			////////////Full Image////////////////////////


			////////////Half Image////////////////////////
			shared_ptr<Material> charHalfImage = make_shared<Material>();
			SetupUIMaterial(charHalfImage);

			tag = L"Char" + prefixTagHalf + L"_" + characterNames[i] + L"_S00" + to_wstring(j);
			path = prefixPath + characterNames[i] + L"\\" + tag + L".png";
			auto charHalfTexture = RESOURCES->Load<Texture>(tag, path);

			charHalfImage->SetDiffuseMap(charHalfTexture);
			MaterialDesc& charHalfImageDesc = charHalfImage->GetMaterialDesc();
			charHalfImageDesc.ambient = Vec4(1.f);
			charHalfImageDesc.diffuse = Vec4(1.f);
			charHalfImageDesc.specular = Vec4(1.0f);
			RESOURCES->Add(tag, charHalfImage);
			////////////Half Image////////////////////////
		}
	}
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

	// 패널에 ImageUI 추가
	auto imageUI = m_backPanel->GetUIPanel()->AddImageUI(Vec2(0, 0), L"MainImageUI");
	// ImageUI에 이미지 레이어들 추가
	imageUI->AddImageLayer(0, Vec2(974, 341), Vec2(490, 540),
		RESOURCES->Get<Material>(L"CSSceneBackGroundDeco_1"), 1);

	m_backPanel->SetLayerIndex(LAYER_UI);

	AddUIObject(m_backPanel, true);
	RegisterUIParent(m_backPanel);
}

void CharacterSelectScene::CreateScrollableCharacterList()
{
	// ScrollView 생성
	m_characterList = make_shared<GameObject>();
	m_characterList->SetName(L"CharacterScrollView");

	auto scrollView = make_shared<ScrollView>();
	m_characterList->AddComponent(scrollView);

	// 화면 중앙에 400x600 크기의 스크롤뷰 생성
	float width = GRAPHICS->GetViewport().GetWidth();
	float height = GRAPHICS->GetViewport().GetHeight();
	Vec2 scrollPos = Vec2(280, 340);
	Vec2 scrollSize = Vec2(500, 415);

	scrollView->Create(scrollPos, scrollSize, nullptr);
	scrollView->SetScrollDirection(ScrollDirection::Vertical);
	scrollView->SetContentSize(Vec2(500.0f, 1800.0f)); // 컨텐츠는 더 큰 크기
	scrollView->SetScrollSpeed(30.0f);
	scrollView->SetPixelClipping(true);	// 픽셀 클리핑 활성화

	int characterIndex = 0;
	for (int i = 0; i < 20; i++)
	{
		Vec2 cardPos = Vec2(-208 + (i % 5) * 100, -140 + (i / 5) * 122); // 세로로 배치
		Vec2 cardSize = Vec2(106, 166);

		shared_ptr<UIPanel> panel = scrollView->AddPanel(cardPos, cardSize, nullptr);
		panel->GetGameObject()->SetName(L"CharListPanel" + to_wstring(i));

		//랜덤버튼
		if (i == 0)
		{
			//=======================버튼==============================//
			shared_ptr<Material> cloneMaterial_RandomSlotNormal = RESOURCES->Get<Material>(L"CharRandomSlotNormal")->Clone();
			shared_ptr<Material> cloneMaterial_RandomSlotRollOver = RESOURCES->Get<Material>(L"CharRandomSlotRollOver")->Clone();

			auto button = panel->AddButton(Vec2(61, 79), Vec2(122, 158),
				cloneMaterial_RandomSlotNormal, L"CharRandomSlotNormal");
			// 버튼 상태별 Material 설정
			button->SetNormalMaterial(cloneMaterial_RandomSlotNormal);
			button->SetHoveredMaterial(cloneMaterial_RandomSlotRollOver);
			button->SetPressedMaterial(cloneMaterial_RandomSlotRollOver);
			//=======================버튼==============================//

			//=======================텍스트==============================//
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
			//=======================텍스트==============================//

			//=======================이미지==============================//
			shared_ptr<Material> cloneMaterial_RandomIcon = RESOURCES->Get<Material>(L"CharLobbyRandom")->Clone();
			auto imageUI = panel->AddImageUI(Vec2(0.f, 0.f));
			imageUI->AddImageLayer(0, Vec2(60, 78), Vec2(42, 64), cloneMaterial_RandomIcon, 1);
			//=======================이미지==============================//
		}
		//나머지 캐릭터버튼
		else
		{
			//=======================버튼==============================//
			//버튼 머테리얼 복제
			shared_ptr<Material> cloneMaterial_SlotNormal = RESOURCES->Get<Material>(L"CharSlotNormal")->Clone();
			shared_ptr<Material> cloneMaterial_SlotRollOver = RESOURCES->Get<Material>(L"CharSlotRollOver")->Clone();

			auto button = panel->AddButton(Vec2(61, 79), Vec2(122, 158),
				cloneMaterial_SlotNormal, L"CharSlotNormal");
			button->GetGameObject()->SetName(characterNames[i - 1]);

			// 버튼 상태별 Material 설정
			button->SetNormalMaterial(cloneMaterial_SlotNormal);
			button->SetHoveredMaterial(cloneMaterial_SlotRollOver);
			button->SetPressedMaterial(cloneMaterial_SlotRollOver);

			// Delegate에 함수 등록 (함수 호출이 아님!)
			button->OnClick += [this, button, i]() {
				UpdateFullImage(button, 0);
				UpdateSkinList(button, i);
			};
			//=======================버튼==============================//
			
			//=======================텍스트==============================//
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
			//=======================텍스트==============================//
			
			//=======================이미지==============================//
			wstring charTag = L"CharLobby" + characterNames[characterIndex++];
			shared_ptr<Material> cloneMaterial_CharLobbyImage = RESOURCES->Get<Material>(charTag)->Clone();
			auto imageUI = panel->AddImageUI(Vec2(0.f, 0.f));
			imageUI->AddImageLayer(0, Vec2(60, 78), Vec2(121, 157), cloneMaterial_CharLobbyImage, 1);
			//=======================이미지==============================//
		}
		
	}

	AddUIObject(m_characterList, true);
	RegisterUIParent(m_characterList);
}

void CharacterSelectScene::CreateScrollableSkinList()
{	
	m_selectedCharacterSkinScrollView = make_shared<GameObject>();
	m_selectedCharacterSkinScrollView->SetName(L"SelectedCharacterSkinScrollView");

	shared_ptr<ScrollView> scrollView = make_shared<ScrollView>();
	m_selectedCharacterSkinScrollView->AddComponent(scrollView);

	Vec2 scrollPos = Vec2(820, 510);
	Vec2 scrollSize = Vec2(400, 233 * RESOLUTION_CONSTANT + 5);

	scrollView->Create(scrollPos, scrollSize, nullptr);
	scrollView->SetScrollDirection(ScrollDirection::Horizontal);
	scrollView->SetContentSize(Vec2(3000.0f, 233 * RESOLUTION_CONSTANT + 5)); // 컨텐츠는 더 큰 크기
	scrollView->SetScrollSpeed(30.0f);
	scrollView->SetPixelClipping(true); // 픽셀 클리핑 활성화

	////강제로 스크롤뷰가 더 앞쪽으로
	Vec3 scrollViewPos = m_selectedCharacterSkinScrollView->GetScrollView()->GetTransform()->GetPosition();
	scrollViewPos.z = 0.3;
	m_selectedCharacterSkinScrollView->GetScrollView()->GetTransform()->SetPosition(scrollViewPos);
	m_selectedCharacterSkinScrollView->GetMeshRenderer()->SetActive(false); //버튼 누르기전까지 비활성화

	AddUIObject(m_selectedCharacterSkinScrollView, true);
	RegisterUIParent(m_selectedCharacterSkinScrollView);
}

void CharacterSelectScene::UpdateSkinList(shared_ptr<Button> button, int charIndex)
{
	//스크롤뷰 요소들 전부삭제 ( 동적으로 바뀌기 때문에 삭제 해줘야 이전께 안남음 )
	m_selectedCharacterSkinScrollView->GetMeshRenderer()->SetActive(true);
	m_selectedCharacterSkinScrollView->GetScrollView()->RemoveAllElement();
	m_selectedCharacterSkinScrollView->GetScrollView()->SetContentSize(Vec2(skinCount[charIndex - 1] * 110, 150));

	shared_ptr<ScrollView> scrollView = m_selectedCharacterSkinScrollView->GetScrollView();
	for (int i = 0; i < skinCount[charIndex - 1]; i++)
	{
		Vec2 cardPos = Vec2(-150 + 110 * i, 0);
		Vec2 cardSize = Vec2(80, 130);

		shared_ptr<Material> clonMaterial_SkinSlotNormal = RESOURCES->Get<Material>(L"CharSkinSlotNormal")->Clone();
		shared_ptr<Material> clonMaterial_SkinSlotRollOver = RESOURCES->Get<Material>(L"CharSkinSlotRollOver")->Clone();
		shared_ptr<UIPanel> panel = scrollView->AddPanel(cardPos, cardSize, nullptr);	

		//=======================버튼==============================//
		// 패널에 버튼도 추가 가능
		auto button = panel->AddButton(Vec2(45, 68), Vec2(156, 233),
			clonMaterial_SkinSlotNormal, L"CharSkinSlotNormal");

		button->SetZPos(0.25);
		button->GetGameObject()->SetName(characterNames[charIndex - 1]);
	
		// 버튼 상태별 Material 설정
		button->SetNormalMaterial(clonMaterial_SkinSlotNormal);
		button->SetHoveredMaterial(clonMaterial_SkinSlotRollOver);
		button->SetPressedMaterial(clonMaterial_SkinSlotRollOver);

		// Delegate에 함수 등록 (함수 호출이 아님!)
		button->OnClick += [this, button, i]() {
			UpdateFullImage(button, i);
			};
		//=======================버튼==============================//

		//=======================이미지==============================//
		wstring tag = L"CharHalf_" + characterNames[charIndex - 1] + L"_S00" + to_wstring(i);
		shared_ptr<Material> clonMaterial_SkinHalfImage = RESOURCES->Get<Material>(tag)->Clone();
		auto imageUI = panel->AddImageUI(Vec2(0.f, 0.f));
		imageUI->SetZPos(0.24);
		imageUI->AddImageLayer(0, Vec2(45, 68), Vec2(140, 220), clonMaterial_SkinHalfImage, 1);
		//=======================이미지==============================//
	}
}

void CharacterSelectScene::UpdateFullImage(shared_ptr<Button> button, int skinIndex)
{
	////Full Image 변경 ////
	wstring buttonName = button->GetGameObject()->GetName();
	buttonName = L"CharFull_" + buttonName + L"_S00" + to_wstring(skinIndex);

	shared_ptr<Material> fullImage = RESOURCES->Get<Material>(buttonName)->Clone();
	Vec2 newSize = fullImage->GetDiffuseMap()->GetSize();

	newSize.x *= RESOLUTION_CONSTANT;
	newSize.y *= RESOLUTION_CONSTANT;

	auto imageUI = m_backPanel->GetUIPanel()->GetImageUI(L"MainImageUI");
	imageUI->SetMaterial(0, fullImage);
	imageUI->SetLayerSize(0, newSize);
	imageUI->SetLayerPosition(0, Vec2(1366 - (newSize.x / 2.f), 768 - (newSize.y / 2.f)));
	////Full Image 변경 ////
}


