#pragma once
#include "Shader.h"


constexpr float UPDATETIME = 0.016f;

class FOWShaderManager
{
    DECLARE_SINGLE(FOWShaderManager);

    ~FOWShaderManager();

public:
    void Init();
    void End();

public:
    void SetFogOfWarData(const FogOfWarData& _data);
    void ApplyToMaterial(shared_ptr<Material> _material);
    void UpdateShaderConstants();

    shared_ptr<Shader> GetFOWShader() { return m_fowShader; }
    bool IsInitialzed() { return m_isInitialized; }

private:
    void CreateFOWShader();
    void CreateConstantBuffer();
private:
    shared_ptr<Shader> m_fowShader;
    ComPtr<ID3D11Buffer> m_fowConstantBuffer;
    ComPtr<ID3DX11EffectConstantBuffer> m_fowEffectBuffer;

    FogOfWarData m_fowData;
    bool m_needsUpdate = true;
    bool m_isInitialized = false;

    float m_curTime = 0.f;
};

