#pragma once
#include "Component.h"

class Model;

struct AnimTransform {

    // 250���� ���� ������ ����� �� ����.
    using TransformArrayType = array<Matrix, MAX_BONE_TRANSFORMS>;

    //250���� �� * 500���� Ű������. 
    //2�� �迭. 
    array<TransformArrayType, MAX_MODEL_KEYFRAMES> transforms;
};

class ModelAnimator :
    public Component
{
    using Super = Component;

public:
    ModelAnimator(shared_ptr<Shader> _shader);
    ~ModelAnimator();

    virtual void Update() override;
    void UpdateTweenData();
    void SetModel(shared_ptr<Model> _model);
    void SetPass(uint8 _pass) { m_pass = _pass; }

    void RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer);
    InstanceID GetInstanceID();
    TweenDesc GetTweenDesc() { return m_tweenDesc; }

private:
    void CreateTexture();
    void CreateAnimationTransform(uint32 _index);

private:
    vector<AnimTransform> m_animTransform;
    ComPtr<ID3D11Texture2D> m_texture;
    ComPtr<ID3D11ShaderResourceView> m_srv;

private:
    TweenDesc m_tweenDesc;

private:
    shared_ptr<Shader>  m_shader;
    uint8               m_pass = 0;
    shared_ptr<Model>   m_model;
};

