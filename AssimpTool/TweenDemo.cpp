#include "pch.h"
#include "TweenDemo.h"
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
#include "ModelAnimator.h"

void TweenDemo::Init()
{
	RESOURCES->Init();
	m_shader = make_shared<Shader>(L"17. TweenDemo.fx");

	// Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());

	Kachujin();
	//CreateTower();
	RENDER->init(m_shader);
}

void TweenDemo::Update()
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

void TweenDemo::Render()
{
}

void TweenDemo::Kachujin()
{
	shared_ptr<class Model> m1 = make_shared<Model>();
	m1->ReadModel(L"Kachujin/Kachujin");
	m1->ReadMaterial(L"Kachujin/Kachujin");
	
	m1->ReadAnimation(L"Kachujin/Idle");
	m1->ReadAnimation(L"Kachujin/Run");
	m1->ReadAnimation(L"Kachujin/Slash");

	m_obj = make_shared<GameObject>();
	m_obj->GetTransform()->SetPosition(Vec3(0, 0, 1));
	m_obj->GetTransform()->SetScale(Vec3(0.01f));

	m_obj->AddComponent(make_shared<ModelAnimator>(m_shader));
	{
		//m_obj->GetModelRenderer()->SetModel(m1);
		m_obj->GetModelAnimator()->SetModel(m1);
		//m_obj->GetModelAnimator()->SetPass(1);
	}

}

