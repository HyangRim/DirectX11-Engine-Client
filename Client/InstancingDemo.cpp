#include "pch.h"
#include "InstancingDemo.h"
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

void InstancingDemo::Init()
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


		//INSTANCING

		m_material = material;

	}

	for (int32 idx = 0; idx < 100000; ++idx) {
		auto obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->SetPosition(Vec3(rand() % 100, 0, rand() % 10));
		obj->AddComponent(make_shared<MeshRenderer>());

		{
			obj->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"Veigar"));
		}
		{
			auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
			obj->GetMeshRenderer()->SetMesh(mesh);

			m_mesh = mesh;
		}
		m_objs.push_back(obj);
	}

	//CreateTower();
	RENDER->init(m_shader);


	//INSTANCING
	m_instanceBuffer = make_shared<VertexBuffer>();

	for (const auto& obj : m_objs) {
		Matrix world = obj->GetTransform()->GetWorldMatrix();
		m_worlds.push_back(world);
	}

	m_instanceBuffer->Create(m_worlds, /*slot*/1);


}

void InstancingDemo::Update()
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

	/*for (const auto& obj : m_objs) {
		obj->Update();
	}*/

	m_material->Update();


	//월드는 자기 자신에서 넣어주는 게 아니라, 인스턴스 버퍼에서 넣어줘야함. 
	// 
	//auto world = GetTransform()->GetWorldMatrix();
	//RENDER->PushTransformData(TransformDesc{ world });
	//m_shader->GetConstantBuffer()

	m_mesh->GetVertexBuffer()->PushData();
	m_instanceBuffer->PushData();

	m_mesh->GetIndexBuffer()->PushData();

	m_shader->DrawIndexedInstanced(0, 0, m_mesh->GetIndexBuffer()->GetCount(), m_objs.size());
}

void InstancingDemo::Render()
{
}
