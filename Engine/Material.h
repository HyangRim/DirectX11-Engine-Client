#pragma once
#include "ResourceBase.h"



//불투명 오브젝트는 먼저 렌더해서 깊이 버퍼에 깊이 값을 기록한다 
//이후 보이지 않는 픽셀은 자동으로 걸러짐.

//Transparent 오브젝트는 후속 픽셀이 가려짐. 따라서 깊이 쓰기를 하지 않고, 
// 가장 뒤에서 앞으로 그려야 올바른 화면을 얻는다. 

enum class RenderQueue {
    Opaque,     //불투명
    Cutout,     // 투명. 
    Transparent,
    Max
};

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
    void SetCubeMap(shared_ptr<Texture> _cubeMap) { m_cubeMap = _cubeMap; }


    void SetRenderQueue(RenderQueue _renderQueue) { m_renderQueue = _renderQueue; }
    RenderQueue GetRenderQueue() { return m_renderQueue; }
    
    void SetCastShadow(bool _castShadow) { m_castShadow = _castShadow; }
    bool GetCastShadow() { return m_castShadow; }

    void Update();

    shared_ptr<Material> Clone();

private:
    friend class MeshRenderer;
    MaterialDesc m_desc;

    RenderQueue m_renderQueue = RenderQueue::Opaque;
    bool m_castShadow = true;

    shared_ptr<Shader> m_shader;


    shared_ptr<Texture> m_diffuseMap;
    shared_ptr<Texture> m_normalMap;
    shared_ptr<Texture> m_specularMap;
    shared_ptr<Texture> m_randomMap;
    shared_ptr<Texture> m_cubeMap;

    ComPtr<ID3DX11EffectShaderResourceVariable> m_diffuseEffectBuffer;
    ComPtr<ID3DX11EffectShaderResourceVariable> m_normalEffectBuffer;
    ComPtr<ID3DX11EffectShaderResourceVariable> m_specularEffectBuffer;
    ComPtr<ID3DX11EffectShaderResourceVariable> m_randomEffectBuffer;
    ComPtr<ID3DX11EffectShaderResourceVariable> m_cubeMapEffectBuffer;
    ComPtr<ID3DX11EffectShaderResourceVariable> m_shadowMapEffectBuffer;
};

