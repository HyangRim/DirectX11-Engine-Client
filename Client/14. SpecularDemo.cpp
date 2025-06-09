#include "pch.h"
#include "GeometryHelper.h"
#include "14. SpecularDemo.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RenderManager.h"

//Depth Stencil�� �߿���. 

void SpecularDemo::Init()
{

	RESOURCES->Init();

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
		auto shader = make_shared<Shader>(L"11. Specular.fx");
		m_shader = shader;
		m_obj->GetMeshRenderer()->SetShader(shader);
	}
	{
		auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
		m_obj->GetMeshRenderer()->SetMesh(mesh);
	}
	{
		auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
		m_obj->GetMeshRenderer()->SetTexture(texture);
	}

	//Object2
	m_obj2 = make_shared<GameObject>();
	m_obj2->GetOrAddTransform()->SetPosition(Vec3(0.5f, 0.f, 2.f));
	m_obj2->AddComponent(make_shared<MeshRenderer>());
	{
		m_obj2->GetMeshRenderer()->SetShader(m_shader);
	}
	{
		auto mesh = RESOURCES->Get<Mesh>(L"Cube");
		m_obj2->GetMeshRenderer()->SetMesh(mesh);
	}
	{
		auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
		m_obj2->GetMeshRenderer()->SetTexture(texture);
	}

	//���ٽ� �䰡 ������, Object�� �������� ��������, �� Z���� ����� ���� ����. 
	//�и��� ť�갡 �� �տ� �־�� �ϴµ�, ����� �׷� ������ �ȵ�. 
	//�����ϰ� ��ü�� �׷����� �ϴ���, �׷����� ���� ���ƾ� �ϳ�.
	//�ȼ� ������ ������ �� �տ� ��ü�� �ִٸ� �� �׸��� ��. 
	RenderManager::GetInstance()->init(m_shader);
}

void SpecularDemo::Update()
{
	m_camera->Update();
	RenderManager::GetInstance()->Update();

	Vec4 light{ 0.5f };
	m_shader->GetVector("LightSpecular")->SetFloatVector((float*)&light);

	Vec3 lightDir{ 1.f, 0.f, 0.f };
	lightDir.Normalize();
	m_shader->GetVector("LightDir")->SetFloatVector((float*)&lightDir);
	//RENDER->Update();
	//RENDER->Update();
	{
		Vec4 material(1.f);
		m_shader->GetVector("MaterialSpecular")->SetFloatVector((float*)&material);
		m_obj->Update();
	}
	{
		Vec4 material(1.f);
		m_shader->GetVector("MaterialSpecular")->SetFloatVector((float*)&material);
		m_obj2->Update();
	}
}

void SpecularDemo::Render()
{

}
