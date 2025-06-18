#pragma once
#include "Viewport.h"

class Graphics
{
	DECLARE_SINGLE(Graphics);

public:
	void Init(HWND hwnd);

	void RenderBegin();
	void RenderEnd();

	ComPtr<ID3D11Device> GetDevice() { return m_device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return m_deviceContext; }

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

public:
	void SetViewport(float _width, float _height, float _x = 0, float _y = 0, float _minDepth = 0, float _maxDepth = 1);
	Viewport& GetViewport() { return m_viewport; }
private:
	HWND m_hwnd = {};

	// Device & SwapChain
	ComPtr<ID3D11Device> m_device = nullptr;
	ComPtr<ID3D11DeviceContext> m_deviceContext = nullptr;
	ComPtr<IDXGISwapChain> m_swapChain = nullptr;

	// RTV
	//���̴����� ����� �ؼ� �׷��ִ� ��. 
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	//DSV(Depth Stencil View)
	//�׸��ڴ� ī�޶� ���� ��ġ���� �� �� �� ���� �� ������
	//���� ���� ������� �׸�. 
	ComPtr<ID3D11Texture2D> m_depthStencilTexture;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	// Misc
	//D3D11_VIEWPORT m_viewport = { 0 };
	Viewport m_viewport;
};

