#include "pch.h"
#include "AnimInstancingDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RenderManager.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Model.h"
#include "Material.h"
#include "RenderDemo.h"

void RenderDemo::Init()
{
	RESOURCES->Init();
	m_shader = make_shared<Shader>(L"23. RenderDemo.fx");

	// Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());

	shared_ptr<class Model> m1 = make_shared<Model>();
	m1->ReadModel(L"Kachujin/Kachujin");
	m1->ReadMaterial(L"Kachujin/Kachujin");
	m1->ReadAnimation(L"Kachujin/Idle");
	m1->ReadAnimation(L"Kachujin/Run");
	m1->ReadAnimation(L"Kachujin/Slash");

	

	for (int32 idx = 0; idx < 500; ++idx) {
		auto obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->SetPosition(Vec3(rand() % 100, 0, rand() % 100));
		obj->GetOrAddTransform()->SetScale(Vec3(0.01f));
		obj->AddComponent(make_shared<ModelAnimator>(m_shader));
		{
			obj->GetModelAnimator()->SetModel(m1);
			obj->GetModelAnimator()->SetPass(2);
			//obj->GetModelRenderer()->SetModel(m1);
		}
		m_objs.push_back(obj);
	}

	//Model
	shared_ptr<class Model> m2 = make_shared<Model>();
	m2->ReadModel(L"Tower/Tower");
	m2->ReadMaterial(L"Tower/Tower");


	for (int32 idx = 0; idx < 500; ++idx) {
		auto obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->SetPosition(Vec3(rand() % 100, 0, rand() % 100));
		obj->GetOrAddTransform()->SetScale(Vec3(0.01f));
		obj->AddComponent(make_shared<ModelRenderer>(m_shader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		m_objs.push_back(obj);
	}


	//CreateTower();
	RENDER->init(m_shader);
}

void RenderDemo::Update()
{
	m_camera->Update();
	RENDER->Update();
	{
		LightDesc lightDesc;
		lightDesc.ambient = Vec4(0.4f);
		lightDesc.diffuse = Vec4(1.f);
		lightDesc.specular = Vec4(0.f);
		lightDesc.direction = Vec3(1.f, 0.f, 1.f);
		RENDER->PushLightData(lightDesc);
	}

	INSTANCING->Render(m_objs);
}

void RenderDemo::Render()
{
}
