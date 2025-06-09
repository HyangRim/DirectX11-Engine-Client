#pragma once
#include "ResourceBase.h"

class Texture : public ResourceBase
{
	using Super = ResourceBase;
public:
	Texture();
	~Texture();

	ComPtr<ID3D11ShaderResourceView> GetComPtr() { return m_shaderResourveView; }

	virtual void Load(const wstring& _path) override;

	Vec2 GetSize() { return m_size; }

	const DirectX::ScratchImage& GetInfo() { return m_img; }

private:
	ComPtr<ID3D11ShaderResourceView> m_shaderResourveView;
	Vec2 m_size = {0.f, 0.f};
	DirectX::ScratchImage m_img = {};
};

