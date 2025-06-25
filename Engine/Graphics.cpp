#include "pch.h"
#include "Graphics.h"

#define SHADOWMAP_SIZE 4096

void Graphics::Init(HWND hwnd)
{
	m_hwnd = hwnd;

	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();
	SetViewport(GAME->GetGameDesc().width, GAME->GetGameDesc().height);
}

void Graphics::RenderBegin()
{
	//������ �ܰ迡�� �׷��� ������� ������ �� ����. 
	//������ ���ڿ� depthStencil�� ���� �ʾ���. ���� ���������� ���̰��� ä���. 

	//���� ���� 0 ~ 1
	//���� �׸� �� ���� ���� 0.5�ε�, �̹� �׷��� ���� 0.35�� �� �ȼ��� ��ŵ. 
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), (float*)(&GAME->GetGameDesc().clearColor));

	//���� �ʱⰪ�� �� 1�� �����ϴ°�? ���̰� 1�̸� �� �ڿ� �ִ� ��. 
	//����� �� 0 ~ 1 �� ��. ���̴� ���� ��ü�� �׷����� �� �׷����� �����ϴ� Z ���� ���̱���.
	
	//���ٽ��̶�. ���ϴ� ������ ���� �װ� ������ �շ� �� �κи� �ٲ۴ٰų�.
	//�׷� ��� ���. 
	//m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	//m_deviceContext->RSSetViewports(1, &m_viewport);

	//���ٽ� ��� ī�޶󸶴� �ʱ�ȭ �ϵ���.  
	m_viewport.RSSetViewport();
}


void Graphics::RenderEnd()
{
	HRESULT hr = m_swapChain->Present(0, 0);
	CHECK(hr);
}

void Graphics::ClearDepthStencilView()
{
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void Graphics::ClearShadowDepthStencilView()
{
	m_deviceContext->ClearDepthStencilView(m_shadowDSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void Graphics::SetShadowDepthStencilView()
{
	m_shadowVP.RSSetViewport();

	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	m_deviceContext->OMSetRenderTargets(1, renderTargets, m_shadowDSV.Get());
}

void Graphics::SetRTVAndDSV()
{
	m_viewport.RSSetViewport();
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
		m_depthStencilView.Get());
}

void Graphics::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		desc.BufferDesc.Width = GAME->GetGameDesc().width;
		desc.BufferDesc.Height = GAME->GetGameDesc().height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//��ĵ ���� �׸��� ���
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		//ũ�� ���� ���
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = m_hwnd;
		desc.Windowed = TRUE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		m_swapChain.GetAddressOf(),
		m_device.GetAddressOf(),
		nullptr,
		m_deviceContext.GetAddressOf()
	);

	CHECK(hr);
}
//RenderTargetView�� DX�� ���������ο��� ������ Ÿ���� �����ϱ� ���� ���ϵ� �������̽�. 
void Graphics::CreateRenderTargetView()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

	hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
	CHECK(hr);
}

void Graphics::CreateDepthStencilView()
{
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		ZeroMemory(&desc, sizeof(desc));

		desc.Width = static_cast<uint32>(GAME->GetGameDesc().width);
		desc.Height = static_cast<uint32>(GAME->GetGameDesc().height);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		//depth stencil �뵵�� ����. 
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = DEVICE->CreateTexture2D(&desc, nullptr, m_depthStencilTexture.GetAddressOf());
		CHECK(hr);
		desc.Width = SHADOWMAP_SIZE;
		desc.Height = SHADOWMAP_SIZE;
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		hr = DEVICE->CreateTexture2D(&desc, nullptr, m_shadowDSTexture.GetAddressOf());
		CHECK(hr);
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Flags = 0;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = DEVICE->CreateDepthStencilView(m_depthStencilTexture.Get(), &desc, m_depthStencilView.GetAddressOf());
		CHECK(hr);

		hr = DEVICE->CreateDepthStencilView(m_shadowDSTexture.Get(), &desc, m_shadowDSV.GetAddressOf());
		CHECK(hr);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		ComPtr<ID3D11ShaderResourceView> srv;
		HRESULT hr = DEVICE->CreateShaderResourceView(m_shadowDSTexture.Get(), &srvDesc, srv.GetAddressOf());
		CHECK(hr);

		m_shadowMap = make_shared<Texture>();
		m_shadowMap->SetSRV(srv);
	}

}

void Graphics::SetViewport(float _width, float _height, float _x, float _y, float _minDepth, float _maxDepth)
{
	m_viewport.Set(_width, _height, _x, _y, _minDepth, _maxDepth);
	m_shadowVP.Set(SHADOWMAP_SIZE, SHADOWMAP_SIZE, _x, _y, _minDepth, _maxDepth);
}
