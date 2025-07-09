#include "pch.h"
#include "UITestDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
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

void UITestDemo::Init()
{	
//	CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\snowcube1024.dds", L"Sky.fx"));
	shared_ptr<Shader> renderShader = make_shared<Shader>(L"23. RenderDemo.fx");
	{
		// Camera
		auto camera = make_shared<GameObject>();
		camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetCamera()->SetNear(1.f);
		camera->GetCamera()->SetFar(100.f);
		camera->GetCamera()->SetProjectionType(ProjectionType::Perspective);
		//camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
		CURSCENE->Add(camera);
	
	}

	{
		// Light
		auto light = make_shared<GameObject>();
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

		//for (int32 i = 0; i < 100; i++)
		//{

		//	auto obj = make_shared<GameObject>();
		//	obj->SetName(to_wstring(i));

		//	obj->GetTransform()->SetPosition(Vec3(rand() % 100, 0, rand() % 100));
		//	
		//	//obj->GetTransform()->SetPosition(Vec3(
		//	//	(rand() % 1000) - 500,  // -500 ~ 499
		//	//	0,
		//	//	(rand() % 1000) - 500   // -500 ~ 499
		//	//));


		//	obj->GetTransform()->SetScale(Vec3(1.f));

		//	obj->AddComponent(make_shared<SphereCollider>());
		//	obj->AddComponent(make_shared<Rigidbody>());
		//	obj->GetCollider()->SetOffset(Vec3(0.f, 1.f, 0.f));
		//	obj->GetRigidbody()->SetStatic(true);

		//	/*obj->AddComponent(make_shared<ModelRenderer>(renderShader));
		//	{
		//		obj->GetModelRenderer()->SetModel(m1);
		//		obj->GetModelRenderer()->SetPass(1);
		//	}*/

		//	obj->AddComponent(make_shared<ModelAnimator>(renderShader));
		//	{
		//		obj->GetModelAnimator()->SetModel(m1);
		//		obj->GetModelAnimator()->SetPass(2);
		//	}

		//	CURSCENE->Add(obj);
		//}


		// 여러 클러스터로 나누어 배치
		for (int32 cluster = 0; cluster < 5; cluster++)
		{
			Vec3 clusterCenter = Vec3(
				(cluster % 3 - 1) * 500,  // -200, 0, 200
				0,
				(cluster / 3 - 1) * 500   // -200, 0, 200
			);

			for (int32 i = 0; i < 20; i++)
			{
				auto obj = make_shared<GameObject>();
				obj->SetName(to_wstring(cluster * 100 + i));

				// 클러스터 중심 주변에 배치
				obj->GetTransform()->SetPosition(clusterCenter + Vec3(
					(rand() % 40) - 20,  // 클러스터 내 랜덤
					0,
					(rand() % 40) - 20
				));

				// 나머지 코드...
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
		}
	}
	
	// UI
	{
		//// Material
		//{
		//	shared_ptr<Material> material = make_shared<Material>();
		//	material->SetShader(renderShader);
		//	auto texture = RESOURCES->Load<Texture>(L"BtnImg", L"..\\Resources\\Textures\\UI_Btn\\Img_Item_Slot_Legendary.png");
		//	material->SetDiffuseMap(texture);
		//	MaterialDesc& desc = material->GetMaterialDesc();
		//	desc.ambient = Vec4(1.f);
		//	desc.diffuse = Vec4(1.f);
		//	desc.specular = Vec4(1.f);
		//	RESOURCES->Add(L"BtnImg", material);
		//}

		//// Mesh
		//{
		//	auto obj = make_shared<GameObject>();
		//	obj->AddComponent(make_shared<Button>());
		//	obj->SetName(L"UI");
		//	obj->GetButton()->Create(Vec2(100, 100), Vec2(100, 100), RESOURCES->Get<Material>(L"BtnImg"));

		//	obj->GetButton()->AddOnClickedEvent([obj]() { std::wcout << obj->GetName() << " : picked\n"; });

		//	CUR_SCENE->Add(obj);
		//}
	}

	{
		/*
		// UICamera
		auto camera = make_shared<GameObject>();
		camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
		camera->GetCamera()->SetNear(1.0f);
		camera->GetCamera()->SetFar(100.0f);
		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, false);
		CURSCENE->Add(camera);*/
		
	}
}

void UITestDemo::Update()
{
}

void UITestDemo::Render()
{
}
