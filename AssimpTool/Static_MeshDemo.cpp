#include "pch.h"
#include "Static_MeshDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "RenderManager.h"

void Static_MeshDemo::Init()
{
	RESOURCES->Init();
	m_shader = make_shared<Shader>(L"15. ModelDemo.fx");

	// Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());

	//CreateTower();
	CreateTank();
	RENDER->init(m_shader);
}

void Static_MeshDemo::Update()
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

	{
		m_obj->Update();
	}
}

void Static_MeshDemo::Render()
{
}

void Static_MeshDemo::CreateTower()
{
	// CustomData -> Memory
	shared_ptr<class Model> m1 = make_shared<Model>();
	m1->ReadModel(L"Tower/Tower");
	m1->ReadMaterial(L"Tower/Tower");

	m_obj = make_shared<GameObject>();
	m_obj->GetTransform()->SetPosition(Vec3(0, 0, 50));
	m_obj->GetTransform()->SetScale(Vec3(1.0f));

	m_obj->AddComponent(make_shared<ModelRenderer>(m_shader));
	{
		m_obj->GetModelRenderer()->SetModel(m1);
		m_obj->GetModelRenderer()->SetPass(1);
	}
}

void Static_MeshDemo::CreateTank()
{
	// CustomData -> Memory
	shared_ptr<class Model> m1 = make_shared<Model>();
	m1->ReadModel(L"Tank/Tank");
	m1->ReadMaterial(L"Tank/Tank");

	m_obj = make_shared<GameObject>();
	m_obj->GetTransform()->SetPosition(Vec3(0, 0, 50));
	m_obj->GetTransform()->SetScale(Vec3(1.0f));

	m_obj->AddComponent(make_shared<ModelRenderer>(m_shader));
	{
		m_obj->GetModelRenderer()->SetModel(m1);
		m_obj->GetModelRenderer()->SetPass(1);
	}
}
