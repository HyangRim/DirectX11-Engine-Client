#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "MathUtils.h"
#include <filesystem>

void ResourceManager::Init()
{
	CreateDefaultMesh();
	CreateRandomTexture();
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

void ResourceManager::CreateRandomTexture()
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	// 
	// Create the random data.
	//
	vector<Vec4> randomValues(1024);

	for (int32 i = 0; i < 1024; ++i)
	{
		randomValues[i].x = MathUtils::Random(-1.0f, 1.0f);
		randomValues[i].y = MathUtils::Random(-1.0f, 1.0f);
		randomValues[i].z = MathUtils::Random(-1.0f, 1.0f);
		randomValues[i].w = MathUtils::Random(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues.data();
	initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ComPtr<ID3D11Texture1D> randomTex;
	CHECK(DEVICE->CreateTexture1D(&texDesc, &initData, randomTex.GetAddressOf()));

	//
	// Create the resource view.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ComPtr<ID3D11ShaderResourceView> randomTexSRV;
	CHECK(DEVICE->CreateShaderResourceView(randomTex.Get(), &viewDesc, randomTexSRV.GetAddressOf()));
	texture->SetSRV(randomTexSRV);
	Add(L"RandomTex", texture);
}

shared_ptr<Texture> ResourceManager::GetOrAddTexture(const wstring& _key, const wstring& _path)
{
	shared_ptr<Texture> texture = Get<Texture>(_key);

	if (filesystem::exists(filesystem::path(_path)) == false)
		return nullptr;

	//텍스처를 일단 로드
	texture = Load<Texture>(_key, _path);

	//없으면 새로 만들고.
	if (texture == nullptr)
	{
		texture = make_shared<Texture>();
		texture->Load(_path);
		Add(_key, texture);
	}

	//있으면 보내주기. 
	return texture;
}