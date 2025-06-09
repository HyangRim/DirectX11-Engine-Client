#pragma once
#include "ConstantBuffer.h"

struct GlobalDesc {
	Matrix V = Matrix::Identity;
	Matrix P = Matrix::Identity;
	Matrix VP = Matrix::Identity;
};

struct TransformDesc {
	Matrix W = Matrix::Identity;
};

// Light
struct LightDesc
{
	Color ambient = Color(1.f, 1.f, 1.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(1.f, 1.f, 1.f, 1.f);
	Color emissive = Color(1.f, 1.f, 1.f, 1.f);

	Vec3 direction;
	float padding0;
};

struct MaterialDesc
{
	Color ambient = Color(0.f, 0.f, 0.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(0.f, 0.f, 0.f, 1.f);
	Color emissive = Color(0.f, 0.f, 0.f, 1.f);
};


class Shader;

class RenderManager
{
	DECLARE_SINGLE(RenderManager);

public:
	void init(shared_ptr<Shader> _shader);
	void Update();

	void PushGlobalData(const Matrix& _view, const Matrix& _projection);
	void PushTransformData(const TransformDesc& _desc);
	void PushLightData(const LightDesc& _desc);
	void PushMaterialData(const MaterialDesc& _desc);

private:
	shared_ptr<Shader> m_shader;

	//� ������ �������ŵ� �� ���ѻ�. 
	GlobalDesc m_globalDesc;
	shared_ptr<ConstantBuffer<GlobalDesc>> m_globalBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_globalEffectBuffer;

	TransformDesc m_transformDesc;
	shared_ptr<ConstantBuffer<TransformDesc>> m_transformBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_transformEffectBuffer;

	//�� ���� Buffer
	LightDesc m_lightDesc;
	shared_ptr<ConstantBuffer<LightDesc>> m_lightBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_lightEffectBuffer;

	//���׸��� ���� Buffer
	MaterialDesc m_materialDesc;
	shared_ptr<ConstantBuffer<MaterialDesc>> m_materialBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_materialEffectBuffer;

};

