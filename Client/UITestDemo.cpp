#include "pch.h"
#include "UITestDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "Scene.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Light.h"
#include "Rigidbody.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"
#include "SphereCollider.h"
#include "Billboard.h"
#include "Terrain.h"
#include "Sky.h"
#include "Button.h"
#include "Text.h"
#include "TextButton.h"
#include "UIPanel.h"

void UITestDemo::Init()
{	
//	CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\snowcube1024.dds", L"Sky.fx"));
	shared_ptr<Shader> renderShader = make_shared<Shader>(L"23. RenderDemo.fx");
	{
		// Camera
		auto camera = make_shared<GameObject>();
		camera->SetName(L"MainCamera");
		camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetCamera()->SetNear(1.f);
		camera->GetCamera()->SetFar(100.f);
		camera->GetCamera()->SetProjectionType(ProjectionType::Perspective);
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
		CURSCENE->Add(camera);
	
	}

	{
		// Light
		auto light = make_shared<GameObject>();
		light->SetName(L"Light");
		light->AddComponent(make_shared<Light>());

		LightDesc lightDesc;
		lightDesc.ambient = Vec4(0.4f);
		lightDesc.diffuse = Vec4(1.f);
		lightDesc.specular = Vec4(0.1f);
		lightDesc.direction = Vec3(1.f, -1.f, 1.f);
		//light->GetLight()->SetLightDesc(lightDesc);
		light->GetTransform()->SetRotation(lightDesc.direction);
		light->GetTransform()->SetPosition(Vec3(0.f, -150.f, 0.f));
		static_pointer_cast<Light>(light->GetFixedComponent(ComponentType::Light))->SetLightDesc(lightDesc);
		CURSCENE->Add(light);
	}


	{
		// Animation
		shared_ptr<Model> m1 = make_shared<Model>();

		m1->ReadModel(L"Nicky/Nicky");
		m1->ReadMaterial(L"Nicky/Nicky");
		m1->ReadAnimation(L"Nicky/Nicky_Run");

		for (int32 i = 0; i < 1; i++)
		{

			auto obj = make_shared<GameObject>();
			obj->SetName(to_wstring(i));

			obj->GetTransform()->SetPosition(Vec3(rand() % 100, 0, rand() % 100));
			
			//obj->GetTransform()->SetPosition(Vec3(
			//	(rand() % 1000) - 500,  // -500 ~ 499
			//	0,
			//	(rand() % 1000) - 500   // -500 ~ 499
			//));


			obj->GetTransform()->SetScale(Vec3(1.f));

			obj->AddComponent(make_shared<SphereCollider>());
			obj->AddComponent(make_shared<Rigidbody>());
			obj->GetCollider()->SetOffset(Vec3(0.f, 1.f, 0.f));
			obj->GetRigidbody()->SetStatic(true);

			/*obj->AddComponent(make_shared<ModelRenderer>(renderShader));
			{
				obj->GetModelRenderer()->SetModel(m1);
				obj->GetModelRenderer()->SetPass(1);
			}*/

			obj->AddComponent(make_shared<ModelAnimator>(renderShader));
			{
				obj->GetModelAnimator()->SetModel(m1);
				obj->GetModelAnimator()->SetPass(2);
			}

			CURSCENE->Add(obj);
		}


		//// 여러 클러스터로 나누어 배치
		//for (int32 cluster = 0; cluster < 5; cluster++)
		//{
		//	Vec3 clusterCenter = Vec3(
		//		(cluster % 3 - 1) * 500,  // -200, 0, 200
		//		0,
		//		(cluster / 3 - 1) * 500   // -200, 0, 200
		//	);

		//	for (int32 i = 0; i < 10; i++)
		//	{
		//		auto obj = make_shared<GameObject>();
		//		obj->SetName(to_wstring(cluster * 100 + i));

		//		// 클러스터 중심 주변에 배치
		//		obj->GetTransform()->SetPosition(clusterCenter + Vec3(
		//			(rand() % 40) - 20,  // 클러스터 내 랜덤
		//			0,
		//			(rand() % 40) - 20
		//		));

		//		// 나머지 코드...
		//		obj->GetTransform()->SetScale(Vec3(1.f));

		//		obj->AddComponent(make_shared<SphereCollider>());
		//		obj->AddComponent(make_shared<Rigidbody>());
		//		obj->GetCollider()->SetOffset(Vec3(0.f, 1.f, 0.f));
		//		obj->GetRigidbody()->SetStatic(true);

		//		/*obj->AddComponent(make_shared<ModelRenderer>(renderShader));
		//		{
		//			obj->GetModelRenderer()->SetModel(m1);
		//			obj->GetModelRenderer()->SetPass(1);
		//		}*/

		//		obj->AddComponent(make_shared<ModelAnimator>(renderShader));
		//		{
		//			obj->GetModelAnimator()->SetModel(m1);
		//			obj->GetModelAnimator()->SetPass(2);
		//		}

		//		CURSCENE->Add(obj);
		//	}
		//}
	}
	
	
	// UI
	{
		// Material
		
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(renderShader);
		auto texture = RESOURCES->Load<Texture>(L"BtnImg", L"..\\Resources\\Textures\\UI_Btn\\Img_Item_Slot_Legendary.png");
		material->SetDiffuseMap(texture);
		MaterialDesc& desc = material->GetMaterialDesc();
		desc.ambient = Vec4(1.f);
		desc.diffuse = Vec4(1.f);
		desc.specular = Vec4(1.f);
		RESOURCES->Add(L"BtnImg", material);

		shared_ptr<Material> panelMaterial = make_shared<Material>();
		panelMaterial->SetShader(renderShader);
		auto texture2 = RESOURCES->Load<Texture>(L"PanelImg", L"..\\Resources\\Textures\\UI_Btn\\Img_Item_Slot_Common_MouseOver.png");
		panelMaterial->SetDiffuseMap(texture2);
		MaterialDesc& panelDesc = panelMaterial->GetMaterialDesc();
		panelDesc.ambient = Vec4(0.3f, 0.3f, 0.3f, 0.9f);
		panelDesc.diffuse = Vec4(0.3f, 0.3f, 0.3f, 0.9f);
		panelDesc.specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		RESOURCES->Add(L"PanelImg", panelMaterial);
		

		
		// UIPanel GameObject 생성
		auto panelObj = make_shared<GameObject>();
		panelObj->SetName(L"MainUIPanel");

		// UIPanel 컴포넌트 추가
		auto uiPanel = make_shared<UIPanel>();
		panelObj->AddComponent(uiPanel);

		// 패널 생성 (화면 중앙에 300x200 크기)
		uiPanel->Create(Vec2(600, 400), Vec2(800, 600), panelMaterial);

		// 씬에 추가
		CURSCENE->Add(panelObj);


		
		// 패널 내부에 텍스트들 추가
		auto titleText = uiPanel->AddText(
			Vec2(150, 30),                          // 패널 내 로컬 위치
			L"게임 메뉴",                           // 텍스트 내용
			50.0f,                                  // 폰트 크기
			Vec4(1.0f, 1.0f, 1.0f, 1.0f),          // 흰색 글자
			1.0f,                                   // 투명도
			Vec4(0.0f, 0.0f, 0.0f, 1.0f),          // 검은색 외곽선
			2.0f,                                   // 외곽선 두께
			L"TitleText"                            // 텍스트 이름
		);
		
		

	
		//// Button GameObject (부모)
		//{
		//	auto buttonObj = make_shared<GameObject>();
		//	buttonObj->SetName(L"UI_Button");

		//	// Button 컴포넌트 추가
		//	buttonObj->AddComponent(make_shared<Button>());
		//	buttonObj->GetButton()->Create(Vec2(100, 100), Vec2(100, 100), RESOURCES->Get<Material>(L"BtnImg"));


		//	buttonObj->GetButton()->AddOnClickedEvent([buttonObj]() {
		//		std::wcout << buttonObj->GetName() << " : clicked\n";
		//		});

		//	buttonObj->SetLayerIndex(LAYER_UI);
		//	CURSCENE->Add(buttonObj);
		//}

		//// Text GameObject (별도 객체)
		//{
		//	auto textObj = make_shared<GameObject>();
		//	textObj->SetName(L"UI_Text");

		//	// Text 컴포넌트 추가
		//	shared_ptr<Text> textComponent = make_shared<Text>();
		//	textObj->AddComponent(textComponent);

		//	textComponent->Create(
		//		Vec2(300, 400),                    // Button과 같은 위치
		//		L"버튼 텍스트",                     // 텍스트
		//		20.0f,                            // 크기
		//		Vec4(1.0f, 1.0f, 1.0f, 1.0f),     // 흰색 글자
		//		1.0f,                             // 투명도
		//		Vec4(0.0f, 0.0f, 0.0f, 1.0f),     // 검은색 외곽선
		//		2.0f                              // 외곽선 두께
		//	);

		//	textObj->SetLayerIndex(LAYER_UI);
		//	CURSCENE->Add(textObj);
		//}
	}

	{
		
		// UICamera
		auto camera = make_shared<GameObject>();
		camera->SetName(L"UICamera");
		camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
		camera->GetCamera()->SetNear(1.0f);
		camera->GetCamera()->SetFar(100.0f);
		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, false);
		CURSCENE->Add(camera);
		
	}
	CURSCENE->Start();
}

void UITestDemo::Update()
{
	
}

void UITestDemo::Render()
{
}

void UITestDemo::ShowImguiTransform()
{
}

