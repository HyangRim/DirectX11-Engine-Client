#include "pch.h"
#include "MeshInstancingDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RenderManager.h"
#include "Material.h"

void MeshInstancingDemo::Init()
{
	RESOURCES->Init();
	m_shader = make_shared<Shader>(L"19. InstancingDemo.fx");

	// Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());

	//Material
	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(m_shader);

		auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
		material->SetDiffuseMap(texture);

		MaterialDesc& desc = material->GetMaterialDesc();
		desc.ambient = Vec4(1.f);
		desc.diffuse = Vec4(1.f);
		desc.specular = Vec4(1.f);

		RESOURCES->Add(L"Veigar", material);

	}

	for (int32 idx = 0; idx < 100000; ++idx) {
		auto obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->SetPosition(Vec3(rand() % 1000, 0, rand() % 1000));
		obj->AddComponent(make_shared<MeshRenderer>());

		{
			obj->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"Veigar"));
		}
		{
			auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
			obj->GetMeshRenderer()->SetMesh(mesh);

		}
		m_objs.push_back(obj);
	}

	//CreateTower();
	RENDER->init(m_shader);




}

void MeshInstancingDemo::Update()
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

void MeshInstancingDemo::Render()
{
}
