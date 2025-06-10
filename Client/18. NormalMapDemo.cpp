#include "pch.h"
#include "GeometryHelper.h"
#include "18. NormalMapDemo.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RenderManager.h"
#include "Material.h"

//Depth Stencil�� �߿���. 

void NormalMapDemo::Init()
{

	RESOURCES->Init();
	m_shader = make_shared<Shader>(L"14. NormalMapping.fx");

	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(m_shader);

		auto texture = RESOURCES->Load<Texture>(L"Leather", L"..\\Resources\\Textures\\Leather.jpg");
		material->SetDiffuseMap(texture);

		auto normalTexture = RESOURCES->Load<Texture>(L"LeatherNormal", L"..\\Resources\\Textures\\Leather_Normal.jpg");
		material->SetNormalMap(normalTexture);

		MaterialDesc& desc = material->GetMaterialDesc();
		desc.ambient = Vec4(1.f);
		desc.diffuse = Vec4(1.f);
		desc.specular = Vec4(1.f);

		RESOURCES->Add(L"Leather", material);
	}
	//Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetOrAddTransform()->SetPosition(Vec3(0.f, 0.f, -10.f));
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());

	//Texture
	//m_texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");


	m_obj = make_shared<GameObject>();
	m_obj->GetOrAddTransform();
	m_obj->AddComponent(make_shared<MeshRenderer>());
	{
		auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
		m_obj->GetMeshRenderer()->SetMesh(mesh);
	}
	{
		auto material = RESOURCES->Get<Material>(L"Leather");
		m_obj->GetMeshRenderer()->SetMaterial(material);
	}

	//Object2
	m_obj2 = make_shared<GameObject>();
	m_obj2->GetOrAddTransform()->SetPosition(Vec3(0.5f, 0.f, 2.f));
	m_obj2->AddComponent(make_shared<MeshRenderer>());
	{
		auto mesh = RESOURCES->Get<Mesh>(L"Cube");
		m_obj2->GetMeshRenderer()->SetMesh(mesh);
	}
	{
		auto material = RESOURCES->Get<Material>(L"Leather");
		m_obj2->GetMeshRenderer()->SetMaterial(material);
	}

	//���ٽ� �䰡 ������, Object�� �������� ��������, �� Z���� ����� ���� ����. 
	//�и��� ť�갡 �� �տ� �־�� �ϴµ�, ����� �׷� ������ �ȵ�. 
	//�����ϰ� ��ü�� �׷����� �ϴ���, �׷����� ���� ���ƾ� �ϳ�.
	//�ȼ� ������ ������ �� �տ� ��ü�� �ִٸ� �� �׸��� ��. 
	RenderManager::GetInstance()->init(m_shader);
}

void NormalMapDemo::Update()
{
	m_camera->Update();
	RenderManager::GetInstance()->Update();
	{
		LightDesc lightDesc;
		lightDesc.ambient = Vec4(0.5f);
		lightDesc.diffuse = Vec4(1.f);
		lightDesc.specular = Vec4(1.f, 1.f, 1.f, 1.f);
		lightDesc.direction = Vec3(1.f, 0.f, 1.f);
		RenderManager::GetInstance()->PushLightData(lightDesc);
	}

	//RENDER->Update();
	//RENDER->Update();
	{
		MaterialDesc desc;
		desc.ambient = Vec4(0.2f);
		desc.diffuse = Vec4(1.f);
		desc.specular = Vec4(1.f);
		RenderManager::GetInstance()->PushMaterialData(desc);
		m_obj->Update();
	}
	{
		MaterialDesc desc;
		desc.ambient = Vec4(0.5f);
		desc.diffuse = Vec4(1.f);
		RenderManager::GetInstance()->PushMaterialData(desc);
		m_obj2->Update();
	}
}

void NormalMapDemo::Render()
{

}
