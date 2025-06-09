#include "pch.h"
#include "Texture.h"

Texture::Texture() : Super(ResourceType::Texture)
{

}

Texture::~Texture()
{

}

void Texture::Load(const wstring& _path)
{
	DirectX::TexMetadata md;
	HRESULT hr = ::LoadFromWICFile(_path.c_str(), WIC_FLAGS_NONE, &md, m_img);
	CHECK(hr);

	hr = ::CreateShaderResourceView(DEVICE.Get(), m_img.GetImages(), m_img.GetImageCount(), md, m_shaderResourveView.GetAddressOf());
	CHECK(hr);
	
	m_size.x = md.width;
	m_size.y = md.height;
}

