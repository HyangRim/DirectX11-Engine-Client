#pragma once
#include "ConstantBuffer.h"

struct GlobalDesc {
	Matrix V = Matrix::Identity;
	Matrix P = Matrix::Identity;
	Matrix VP = Matrix::Identity;
	Matrix Vinv = Matrix::Identity;
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
//Bone
#define MAX_BONE_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500

struct BoneDesc {
	Matrix transforms[MAX_BONE_TRANSFORMS];
};

//Animation
struct KeyframeDesc {
	int32 animIndex = 0;
	uint32 currFrame = 0;

	//TODO
	uint32 m_nextFrame = 0;
	float m_ratio = 0.f;
	float m_sumTime = 0.f;
	float m_speed = 1.f;
	Vec2 padding;
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
	void PushBoneData(const BoneDesc& _desc);
	void PushKeyframeData(const KeyframeDesc& _desc);

private:
	shared_ptr<Shader> m_shader;

	//어떤 정보를 넣으려거든 이 삼총사. 
	GlobalDesc m_globalDesc;
	shared_ptr<ConstantBuffer<GlobalDesc>> m_globalBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_globalEffectBuffer;

	TransformDesc m_transformDesc;
	shared_ptr<ConstantBuffer<TransformDesc>> m_transformBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_transformEffectBuffer;

	//색 관련 Buffer
	LightDesc m_lightDesc;
	shared_ptr<ConstantBuffer<LightDesc>> m_lightBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_lightEffectBuffer;

	//마테리얼 관련 Buffer
	MaterialDesc m_materialDesc;
	shared_ptr<ConstantBuffer<MaterialDesc>> m_materialBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_materialEffectBuffer;

	//Born관련 Buffer
	BoneDesc m_boneDesc;
	shared_ptr<ConstantBuffer<BoneDesc>> m_boneBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_boneEffectBuffer;


	KeyframeDesc m_keyframeDesc;
	shared_ptr<ConstantBuffer<KeyframeDesc>> m_keyframeBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_keyframeEffectBuffer;
};

