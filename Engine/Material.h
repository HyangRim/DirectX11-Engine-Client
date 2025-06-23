#pragma once
#include "ResourceBase.h"

class Material :
    public ResourceBase
{
    using Super = ResourceBase;
public:

    Material();
    virtual ~Material();

    shared_ptr<Shader> GetShader() { return m_shader; }

    MaterialDesc& GetMaterialDesc() { return m_desc; }
    shared_ptr<Texture> GetDiffuseMap() { return m_diffuseMap; }
    shared_ptr<Texture> GetNormalMap() { return m_normalMap; }
    shared_ptr<Texture> GetSpecularMap() { return m_specularMap; }
    shared_ptr<Texture> GetRandomTex() { return m_randomMap; }

    void SetShader(shared_ptr<Shader> _shader);
    void SetDiffuseMap(shared_ptr<Texture> _diffuseMap) { m_diffuseMap = _diffuseMap; }
    void SetNormalMap(shared_ptr<Texture> _normalMap) { m_normalMap = _normalMap; }
    void SetSpecularMap(shared_ptr<Texture> _specularMap) { m_specularMap = _specularMap; }
    void SetRandomTex(shared_ptr<Texture> _randomTex) { m_randomMap = _randomTex; }


    void Update();

    shared_ptr<Material> Clone();

private:
    friend class MeshRenderer;
    MaterialDesc m_desc;
    shared_ptr<Shader> m_shader;


    shared_ptr<Texture> m_diffuseMap;
    shared_ptr<Texture> m_normalMap;
    shared_ptr<Texture> m_specularMap;
    shared_ptr<Texture> m_randomMap;

    ComPtr<ID3DX11EffectShaderResourceVariable> m_diffuseEffectBuffer;
    ComPtr<ID3DX11EffectShaderResourceVariable> m_normalEffectBuffer;
    ComPtr<ID3DX11EffectShaderResourceVariable> m_specularEffectBuffer;
    ComPtr<ID3DX11EffectShaderResourceVariable> m_randomEffectBuffer;
};

