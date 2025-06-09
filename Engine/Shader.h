#pragma once
#include "Pass.h"
#include "Technique.h"

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
};

class ShaderManager
{
public:
	static ShaderDesc GetEffect(wstring _fileName);

private:
	static unordered_map<wstring, ShaderDesc> m_shaders;
};