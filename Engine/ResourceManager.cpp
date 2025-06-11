#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include <filesystem>

void ResourceManager::Init()
{
	CreateDefaultMesh();
}


void ResourceManager::CreateDefaultMesh()
{
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateQuad();
		Add(L"Quad", mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateCube();
		Add(L"Cube", mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateSphere();
		Add(L"Sphere", mesh);
	}
}

shared_ptr<Texture> ResourceManager::GetOrAddTexture(const wstring& _key, const wstring& _path)
{
	shared_ptr<Texture> texture = Get<Texture>(_key);

	if (filesystem::exists(filesystem::path(_path)) == false)
		return nullptr;

	//�ؽ�ó�� �ϴ� �ε�
	texture = Load<Texture>(_key, _path);

	//������ ���� �����.
	if (texture == nullptr)
	{
		texture = make_shared<Texture>();
		texture->Load(_path);
		Add(_key, texture);
	}

	//������ �����ֱ�. 
	return texture;
}