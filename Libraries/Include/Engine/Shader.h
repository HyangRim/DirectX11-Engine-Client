#pragma once
#include "Pass.h"
#include "Technique.h"
#include "BindShaderDesc.h"

struct ShaderDesc
{
	ComPtr<ID3DBlob> m_blob;
	ComPtr<ID3DX11Effect> m_effect;
};

class Shader
{
public:
	friend struct Pass;

public:
	Shader(wstring _file);
	~Shader();

	wstring GetFile() { return m_file; }
	ComPtr<ID3DX11Effect> Effect() { return m_shaderDesc.m_effect; }

	void Draw(UINT _technique, UINT _pass, UINT _vertexCount, UINT _startVertexLocation = 0);
	void DrawIndexed(UINT _technique, UINT _pass, UINT _indexCount, UINT _startIndexLocation = 0, INT _baseVertexLocation = 0);
	void DrawInstanced(UINT _technique, UINT _pass, UINT _vertexCountPerInstance, UINT _instanceCount, UINT _startVertexLocation = 0, UINT _startInstanceLocation = 0);
	void DrawIndexedInstanced(UINT _technique, UINT _pass, UINT _indexCountPerInstance, UINT _instanceCount, UINT _startIndexLocation = 0, INT _baseVertexLocation = 0, UINT _startInstanceLocation = 0);

	void Dispatch(UINT _technique, UINT _pass, UINT _x, UINT _y, UINT _z);

	ComPtr<ID3DX11EffectVariable> GetVariable(string _name);
	ComPtr<ID3DX11EffectScalarVariable> GetScalar(string _name);
	ComPtr<ID3DX11EffectVectorVariable> GetVector(string _name);
	ComPtr<ID3DX11EffectMatrixVariable> GetMatrix(string _name);
	ComPtr<ID3DX11EffectStringVariable> GetString(string _name);
	ComPtr<ID3DX11EffectShaderResourceVariable> GetSRV(string _name);
	ComPtr<ID3DX11EffectRenderTargetViewVariable> GetRTV(string _name);
	ComPtr<ID3DX11EffectDepthStencilViewVariable> GetDSV(string _name);
	ComPtr<ID3DX11EffectUnorderedAccessViewVariable> GetUAV(string _name);
	ComPtr<ID3DX11EffectConstantBuffer> GetConstantBuffer(string _name);
	ComPtr<ID3DX11EffectShaderVariable> GetShader(string _name);
	ComPtr<ID3DX11EffectBlendVariable> GetBlend(string _name);
	ComPtr<ID3DX11EffectDepthStencilVariable> GetDepthStencil(string _name);
	ComPtr<ID3DX11EffectRasterizerVariable> GetRasterizer(string _name);
	ComPtr<ID3DX11EffectSamplerVariable> GetSampler(string _name);

private:
	void CreateEffect();
	ComPtr<ID3D11InputLayout> CreateInputLayout(ComPtr<ID3DBlob> _fxBlob, D3DX11_EFFECT_SHADER_DESC* _effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& _params);

private:
	wstring m_file;
	ShaderDesc m_shaderDesc;
	D3DX11_EFFECT_DESC m_effectDesc;
	shared_ptr<StateBlock> m_initialStateBlock;
	vector<Technique> m_techniques;



public:
	//셰이더에서 이걸 직접 관리함. 
	void PushGlobalData(const Matrix& _view, const Matrix& _projection);
	void PushTransformData(const TransformDesc& _desc);
	void PushLightData(const LightDesc& _desc);
	void PushMaterialData(const MaterialDesc& _desc);
	void PushBoneData(const BoneDesc& _desc);
	void PushKeyframeData(const KeyframeDesc& _desc);
	void PushTweenData(const InstancedTweenDesc& _desc);
	void PushSnowData(const SnowBillboardDesc& _desc);
	void PushParticleData(const ParticleDesc& _desc);


private:
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

	InstancedTweenDesc m_tweenDesc;
	shared_ptr<ConstantBuffer<InstancedTweenDesc>> m_tweenBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_tweenEffectBuffer;

	SnowBillboardDesc m_snowDesc;
	shared_ptr<ConstantBuffer<SnowBillboardDesc>> m_snowBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_snowEffectBuffer;

	ParticleDesc m_particleDesc;
	shared_ptr<ConstantBuffer<ParticleDesc>> m_particleBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_particleEffectBuffer;
};

class ShaderManager
{
public:
	static ShaderDesc GetEffect(wstring _fileName);

private:
	static unordered_map<wstring, ShaderDesc> m_shaders;
};