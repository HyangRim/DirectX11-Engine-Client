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
#include "Billboard.h"
#include "Terrain.h"

void UITestDemo::Init()
{	
	//CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\snowcube1024.dds", L"Sky.fx"));
	shared_ptr<Shader> renderShader = make_shared<Shader>(L"23. RenderDemo.fx");
	{
		// Camera
		auto camera = make_shared<GameObject>();
		camera->GetTransform()->SetPosition(Vec3{ 0.f, 2.f, -15.f });
		camera->AddComponent(make_shared<Camera>());
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
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
		/*m1->ReadModel(L"Kachujin/Kachujin");
		m1->ReadMaterial(L"Kachujin/Kachujin");
		m1->ReadAnimation(L"Kachujin/Idle");*/

		m1->ReadModel(L"Bianca2/Bianca");
		m1->ReadMaterial(L"Bianca2/Bianca");
		m1->ReadAnimation(L"Bianca2/Bianca_atk");

		//m1->ReadModel(L"Aya/Aya");
		//m1->ReadMaterial(L"Aya/Aya");
		//m1->ReadAnimation(L"Aya/Aya_Run");

		//m1->ReadAnimation(L"Kachujin/Run");
		//m1->ReadAnimation(L"Kachujin/Slash");

		for (int32 i = 0; i < 1; i++)
		{

			auto obj = make_shared<GameObject>();
			obj->GetTransform()->SetPosition(Vec3(5.f, 1.f, 0.f));
			obj->GetTransform()->SetScale(Vec3(1.f));
			//obj->GetTransform()->SetRotation(Vec3(-180.f, 0.f, 0.f));


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

	//{
	//	// Model
	//	shared_ptr<Model> m2 = make_shared<Model>();
	//	m2->ReadModel(L"Tower/Tower");
	//	m2->ReadMaterial(L"Tower/Tower");

	//	for (int32 i = 0; i < 50; i++)
	//	{
	//		auto obj = make_shared<GameObject>();
	//		obj->GetTransform()->SetPosition(Vec3(rand() % 100, -1, rand() % 100));
	//		obj->GetTransform()->SetScale(Vec3(0.01f));

	//		obj->AddComponent(make_shared<ModelRenderer>(renderShader));
	//		{
	//			obj->GetModelRenderer()->SetModel(m2);
	//			obj->GetModelRenderer()->SetPass(1);
	//		}

	//		CURSCENE->Add(obj);
	//	}
	//}

	

	{
		// UICamera
		auto camera = make_shared<GameObject>();
		camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
		camera->GetCamera()->SetNear(1.0f);
		camera->GetCamera()->SetFar(100.0f);
		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, false);
		CURSCENE->Add(camera);
	}
}

void UITestDemo::Update()
{
}

void UITestDemo::Render()
{
}
