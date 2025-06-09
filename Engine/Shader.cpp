#include "pch.h"
#include "Shader.h"
#include "Utils.h"

Shader::Shader(wstring _file) : m_file(L"..\\Shaders\\" + _file)
{
	m_initialStateBlock = make_shared<StateBlock>();
	{
		DC->RSGetState(m_initialStateBlock->m_RSRasterizerState.GetAddressOf());
		DC->OMGetBlendState(m_initialStateBlock->m_OMBlendState.GetAddressOf(), m_initialStateBlock->m_OMBlendFactor, &m_initialStateBlock->m_OMSampleMask);
		DC->OMGetDepthStencilState(m_initialStateBlock->m_OMDepthStencilState.GetAddressOf(), &m_initialStateBlock->m_OMStencilRef);
	}

	CreateEffect();
}

Shader::~Shader()
{
	
}

void Shader::CreateEffect()
{
	m_shaderDesc = ShaderManager::GetEffect(m_file);

	m_shaderDesc.m_effect->GetDesc(&m_effectDesc);
	for (UINT t = 0; t < m_effectDesc.Techniques; ++t)
	{
		Technique technique;
		technique.m_technique = m_shaderDesc.m_effect->GetTechniqueByIndex(t);
		technique.m_technique->GetDesc(&technique.m_desc);
		technique.m_name = Utils::ToWString(technique.m_desc.Name);

		for (UINT p = 0; p < technique.m_desc.Passes; ++p)
		{
			Pass pass;
			pass.m_pass = technique.m_technique->GetPassByIndex(p);
			pass.m_pass->GetDesc(&pass.m_desc);
			pass.m_name = Utils::ToWString(pass.m_desc.Name);
			pass.m_pass->GetVertexShaderDesc(&pass.m_passVsDesc);
			pass.m_passVsDesc.pShaderVariable->GetShaderDesc(pass.m_passVsDesc.ShaderIndex, &pass.m_effectVsDesc);

			for (UINT s = 0; s < pass.m_effectVsDesc.NumInputSignatureEntries; ++s)
			{
				D3D11_SIGNATURE_PARAMETER_DESC desc;

				HRESULT hr = pass.m_passVsDesc.pShaderVariable->GetInputSignatureElementDesc(pass.m_passVsDesc.ShaderIndex, s, &desc);
				CHECK(hr);

				pass.m_signatureDescs.push_back(desc);
			}

			pass.m_inputLayout = CreateInputLayout(m_shaderDesc.m_blob, &pass.m_effectVsDesc, pass.m_signatureDescs);
			pass.m_stateBlock = m_initialStateBlock;

			technique.m_passes.push_back(pass);
		}

		m_techniques.push_back(technique);
	}

	for (UINT i = 0; i < m_effectDesc.ConstantBuffers; ++i)
	{
		ID3DX11EffectConstantBuffer* iBuffer;
		iBuffer = m_shaderDesc.m_effect->GetConstantBufferByIndex(i);

		D3DX11_EFFECT_VARIABLE_DESC vDesc;
		iBuffer->GetDesc(&vDesc);
	}

	for (UINT i = 0; i < m_effectDesc.GlobalVariables; ++i)
	{
		ID3DX11EffectVariable* effectVariable;
		effectVariable = m_shaderDesc.m_effect->GetVariableByIndex(i);

		D3DX11_EFFECT_VARIABLE_DESC vDesc;
		effectVariable->GetDesc(&vDesc);
	}
}

ComPtr<ID3D11InputLayout> Shader::CreateInputLayout(ComPtr<ID3DBlob> _fxBlob, D3DX11_EFFECT_SHADER_DESC* _effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& _params)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

	for (D3D11_SIGNATURE_PARAMETER_DESC& paramDesc : _params)
	{
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string name = paramDesc.SemanticName;
		std::transform(name.begin(), name.end(), name.begin(), toupper);

		if (name == "POSITION")
		{
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}

		if (Utils::StartsWith(name, "INST") == true)
		{
			elementDesc.InputSlot = 1;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}

		if (Utils::StartsWith(name, "SV_") == false)
			inputLayoutDesc.push_back(elementDesc);
	}

	const void* code = _effectVsDesc->pBytecode;
	UINT codeSize = _effectVsDesc->BytecodeLength;

	if (inputLayoutDesc.size() > 0)
	{
		ComPtr<ID3D11InputLayout> inputLayout;

		HRESULT hr = DEVICE->CreateInputLayout
		(
			&inputLayoutDesc[0]
			, inputLayoutDesc.size()
			, code
			, codeSize
			, inputLayout.GetAddressOf()
		);

		CHECK(hr);

		return inputLayout;
	}

	return nullptr;
}

void Shader::Draw(UINT _technique, UINT _pass, UINT _vertexCount, UINT _startVertexLocation)
{
	m_techniques[_technique].m_passes[_pass].Draw(_vertexCount, _startVertexLocation);
}

void Shader::DrawIndexed(UINT _technique, UINT _pass, UINT _indexCount, UINT _startIndexLocation, INT _baseVertexLocation)
{
	m_techniques[_technique].m_passes[_pass].DrawIndexed(_indexCount, _startIndexLocation, _baseVertexLocation);
}

void Shader::DrawInstanced(UINT _technique, UINT _pass, UINT _vertexCountPerInstance, UINT _instanceCount, UINT _startVertexLocation, UINT _startInstanceLocation)
{
	m_techniques[_technique].m_passes[_pass].DrawInstanced(_vertexCountPerInstance, _instanceCount, _startVertexLocation, _startInstanceLocation);
}

void Shader::DrawIndexedInstanced(UINT _technique, UINT _pass, UINT _indexCountPerInstance, UINT _instanceCount, UINT _startIndexLocation, INT _baseVertexLocation, UINT _startInstanceLocation)
{
	m_techniques[_technique].m_passes[_pass].DrawIndexedInstanced(_indexCountPerInstance, _instanceCount, _startIndexLocation, _baseVertexLocation, _startInstanceLocation);
}

void Shader::Dispatch(UINT _technique, UINT _pass, UINT _x, UINT _y, UINT _z)
{
	m_techniques[_technique].m_passes[_pass].Dispatch(_x, _y, _z);
}

ComPtr<ID3DX11EffectVariable> Shader::GetVariable(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str());
}

ComPtr<ID3DX11EffectScalarVariable> Shader::GetScalar(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsScalar();
}

ComPtr<ID3DX11EffectVectorVariable> Shader::GetVector(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsVector();
}

ComPtr<ID3DX11EffectMatrixVariable> Shader::GetMatrix(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsMatrix();
}

ComPtr<ID3DX11EffectStringVariable> Shader::GetString(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsString();
}

ComPtr<ID3DX11EffectShaderResourceVariable> Shader::GetSRV(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsShaderResource();
}

ComPtr<ID3DX11EffectRenderTargetViewVariable> Shader::GetRTV(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsRenderTargetView();
}

ComPtr<ID3DX11EffectDepthStencilViewVariable> Shader::GetDSV(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsDepthStencilView();
}

ComPtr<ID3DX11EffectConstantBuffer> Shader::GetConstantBuffer(string _name)
{
	return m_shaderDesc.m_effect->GetConstantBufferByName(_name.c_str());
}

ComPtr<ID3DX11EffectShaderVariable> Shader::GetShader(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsShader();
}

ComPtr<ID3DX11EffectBlendVariable> Shader::GetBlend(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsBlend();
}

ComPtr<ID3DX11EffectDepthStencilVariable> Shader::GetDepthStencil(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsDepthStencil();
}

ComPtr<ID3DX11EffectRasterizerVariable> Shader::GetRasterizer(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsRasterizer();
}

ComPtr<ID3DX11EffectSamplerVariable> Shader::GetSampler(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsSampler();
}

ComPtr<ID3DX11EffectUnorderedAccessViewVariable> Shader::GetUAV(string _name)
{
	return m_shaderDesc.m_effect->GetVariableByName(_name.c_str())->AsUnorderedAccessView();
}

unordered_map<wstring, ShaderDesc> ShaderManager::m_shaders;

ShaderDesc ShaderManager::GetEffect(wstring _fileName)
{
	if (m_shaders.count(_fileName) == 0)
	{
		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> error;
		INT flag = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

		HRESULT hr = ::D3DCompileFromFile(_fileName.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, NULL, "fx_5_0", flag, NULL, blob.GetAddressOf(), error.GetAddressOf());
		if (FAILED(hr))
		{
			if (error != NULL)
			{
				string str = (const char*)error->GetBufferPointer();
				MessageBoxA(NULL, str.c_str(), "Shader Error", MB_OK);
			}
			assert(false);
		}

		ComPtr<ID3DX11Effect> effect;
		hr = ::D3DX11CreateEffectFromMemory(blob->GetBufferPointer(), blob->GetBufferSize(), 0, DEVICE.Get(), effect.GetAddressOf());
		CHECK(hr);
		
		m_shaders[_fileName] = ShaderDesc{blob, effect};
	}
	
	ShaderDesc desc = m_shaders.at(_fileName);
	ComPtr<ID3DX11Effect> effect;
	desc.m_effect->CloneEffect(D3DX11_EFFECT_CLONE_FORCE_NONSINGLE, effect.GetAddressOf());

	return ShaderDesc{desc.m_blob, effect};
}
