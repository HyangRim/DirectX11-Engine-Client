#pragma once
#include "Renderer.h"

class Model;
class Shader;
class Material;

class ModelRenderer :
    public Renderer
{
    using Super = Renderer;


public:
    ModelRenderer(shared_ptr<Shader> _shader);
    virtual ~ModelRenderer();

    //virtual void Update() override;

    void SetModel(shared_ptr<Model> _model);
    void SetPass(uint8 _pass) { m_pass = _pass; }

    void RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer, bool _isShadowTech);
    InstanceID GetInstanceID();

    void SetMaterial(shared_ptr <Material> _material) override;

private:
    shared_ptr<Shader>  m_shader;
    uint8               m_pass = 0;
    shared_ptr<Model>   m_model;
};

