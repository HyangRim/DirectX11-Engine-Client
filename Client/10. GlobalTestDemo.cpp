#include "pch.h"
#include "GeometryHelper.h"
#include "10. GlobalTestDemo.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RenderManager.h"

void GlobalTestDemo::Init()
{
	//Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetOrAddTransform();
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());

	//Texture
	//m_texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");


	m_obj = make_shared<GameObject>();
	m_obj->GetOrAddTransform();
	m_obj->AddComponent(make_shared<MeshRenderer>());
	{
		auto shader = make_shared<Shader>(L"08. GlobalTest.fx");
		m_shader = shader;
		m_obj->GetMeshRenderer()->SetShader(shader);
	}
	{
		RESOURCES->Init();
		auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
		m_obj->GetMeshRenderer()->SetMesh(mesh);
	}
	{
		auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
		m_obj->GetMeshRenderer()->SetTexture(texture);
	}

	RenderManager::GetInstance()->init(m_shader);
}

void GlobalTestDemo::Update()
{
	m_camera->Update();
	RenderManager::GetInstance()->Update();
	//RENDER->Update();
	//RENDER->Update();
	m_obj->Update();

}

void GlobalTestDemo::Render()
{

}
