#include "pch.h"
#include "09. MeshDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Texture.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Mesh.h"

void MeshDemo::Init()
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
		auto shader = make_shared<Shader>(L"07. Normal.fx");
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
}

void MeshDemo::Update()
{
	m_camera->Update();
	m_obj->Update();

}

void MeshDemo::Render()
{

}
