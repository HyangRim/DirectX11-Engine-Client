#include "pch.h"
#include "FOWShaderManager.h"
#include "Material.h"

FOWShaderManager::~FOWShaderManager()
{
	End();
}

void FOWShaderManager::Init()
{
	if (m_isInitialized) return;

	CreateFOWShader();
	CreateConstantBuffer();

	m_fowData = {
		Vec3::Zero, 30.f, 0.2f, 8.0f, 2.0f, 0.0f
	};

	m_isInitialized = true;
}

void FOWShaderManager::End()
{
	if(m_fowConstantBuffer != nullptr)
		m_fowConstantBuffer.Reset();
	
	if(m_fowEffectBuffer != nullptr)
		m_fowEffectBuffer.Reset();
	m_isInitialized = false;
}

void FOWShaderManager::SetFogOfWarData(const FogOfWarData& _data)
{
	if (memcmp(&m_fowData, &_data, sizeof(FogOfWarData)) != 0) {
		m_fowData = _data;
		m_fowData.time = GetTickCount64() / 1000.f;
		m_needsUpdate = true;
	}
}

void FOWShaderManager::ApplyToMaterial(shared_ptr<Material> _material)
{
	if (!m_isInitialized || !_material) return;

	_material->SetShader(m_fowShader);
	UpdateShaderConstants();
}

void FOWShaderManager::UpdateShaderConstants()
{
	if (!m_needsUpdate || !m_fowConstantBuffer) return;

	m_curTime += DT;
	if (m_curTime <= UPDATETIME) return;


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = DC->Map(m_fowConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (SUCCEEDED(hr)) {
		memcpy(mappedResource.pData, &m_fowData, sizeof(FogOfWarData));

		DC->Unmap(m_fowConstantBuffer.Get(), 0);
		DC->PSSetConstantBuffers(5, 1, m_fowConstantBuffer.GetAddressOf());

		m_needsUpdate = false;
		m_curTime = 0.f;
	}
}

void FOWShaderManager::CreateFOWShader()
{
	m_fowShader = make_shared<Shader>(L"FOW.fx");
	if (!m_fowShader) {
		assert(false);
	}
}

void FOWShaderManager::CreateConstantBuffer()
{
	if (!m_fowShader) return;

	m_fowEffectBuffer = m_fowShader->GetConstantBuffer("FogOfWarData");

	if (!m_fowEffectBuffer) return;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(FogOfWarData);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	HRESULT hr = DEVICE->CreateBuffer(&bufferDesc, nullptr, m_fowConstantBuffer.GetAddressOf());
	if (FAILED(hr)) {
		assert(false);
	}
}
