#include "pch.h"
#include "RawBuffer.h"


//

RawBuffer::RawBuffer(void* _inputData, uint32 _inputByte, uint32 _outputByte)
	: m_inputData(_inputData), m_inputByte(_inputByte), m_outputByte(_outputByte)
{
	CreateBuffer();
}

RawBuffer::~RawBuffer()
{
}

void RawBuffer::CreateBuffer()
{
	CreateInput();
	CreateSRV();
	CreateOutput();
	CreateUAV();
	CreateResult();
}

void RawBuffer::CopyToInput(void* data)
{
	//INPUT DATA�� CPU -> GPU
	D3D11_MAPPED_SUBRESOURCE subResource;
	DC->Map(m_input.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, m_inputByte);
	}
	DC->Unmap(m_input.Get(), 0);
}

void RawBuffer::CopyFromOutput(void* data)
{
	//OUTPUT DATA�� GPU -> CPU
	// ��� ������ -> result�� ����
	DC->CopyResource(m_result.Get(), m_output.Get());

	D3D11_MAPPED_SUBRESOURCE subResource;
	DC->Map(m_result.Get(), 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(data, subResource.pData, m_outputByte);
	}
	DC->Unmap(m_result.Get(), 0);
}

void RawBuffer::CreateInput()
{
	if (m_inputByte == 0)
		return;

	//����ϱ� ���� ���۸� �����. 
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = m_inputByte;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS; // RAW_BUFFER
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU-WRITE, GPU-READ
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = m_inputData;

	if (m_inputData != nullptr)
		CHECK(DEVICE->CreateBuffer(&desc, &subResource, m_input.GetAddressOf()));
	else
		CHECK(DEVICE->CreateBuffer(&desc, nullptr, m_input.GetAddressOf()));
}

void RawBuffer::CreateSRV()
{
	if (m_inputByte == 0)
		return;

	D3D11_BUFFER_DESC desc;
	m_input->GetDesc(&desc);



	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	//4����Ʈ ��. 
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS; // ���̴����� �˾Ƽ� �ϼ���
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX; // SRV_FLAG_RAW
	srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	srvDesc.BufferEx.NumElements = desc.ByteWidth / 4; // ��ü ������ ����

	CHECK(DEVICE->CreateShaderResourceView(m_input.Get(), &srvDesc, m_srv.GetAddressOf()));
}

void RawBuffer::CreateOutput()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = m_outputByte;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	//output���ٰ� ���ϴ� ���۸� ������ش�. 
	CHECK(DEVICE->CreateBuffer(&desc, NULL, m_output.GetAddressOf()));
}

void RawBuffer::CreateUAV()
{
	//���۸� �����ϴ� uav�� �����. 
	D3D11_BUFFER_DESC desc;
	m_output->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	uavDesc.Buffer.NumElements = desc.ByteWidth / 4;

	CHECK(DEVICE->CreateUnorderedAccessView(m_output.Get(), &uavDesc, m_uav.GetAddressOf()));
}

void RawBuffer::CreateResult()
{
	//�������� ����� cpu�� �������� ���ؼ�
	//result�� ���۸� �����, �� result�� cpu�� �־��� ����. 
	D3D11_BUFFER_DESC desc;
	m_output->GetDesc(&desc);

	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = D3D11_USAGE_DEFAULT; // UAV�� ����Ƿ���, USAGE�� DEFAULT���� ��.
	desc.MiscFlags = 0;

	CHECK(DEVICE->CreateBuffer(&desc, nullptr, m_result.GetAddressOf()));
}
