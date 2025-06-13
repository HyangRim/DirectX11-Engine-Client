#include "pch.h"
#include "SkyDemo.h"
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

void SkyDemo::Init()
{
	RESOURCES->Init();
	m_shader = make_shared<Shader>(L"18. SkyDemo.fx");

	// Material
	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(m_shader);
		auto texture = RESOURCES->Load<Texture>(L"Sky", L"..\\Resources\\Textures\\Sky01.jpg");
		material->SetDiffuseMap(texture);
		MaterialDesc& desc = material->GetMaterialDesc();
		desc.ambient = Vec4(1.f);
		desc.diffuse = Vec4(1.f);
		desc.specular = Vec4(1.f);
		RESOURCES->Add(L"Sky", material);
	}

	{
		// Object
		m_obj = make_shared<GameObject>();
		m_obj->GetOrAddTransform();
		m_obj->AddComponent(make_shared<MeshRenderer>());
		{
			auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
			m_obj->GetMeshRenderer()->SetMesh(mesh);
		}
		{
			auto material = RESOURCES->Get<Material>(L"Sky");
			m_obj->GetMeshRenderer()->SetMaterial(material);
		}
	}

	// Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());

	//CreateTower();
	RENDER->init(m_shader);
}

void SkyDemo::Update()
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

void SkyDemo::Render()
{
}


