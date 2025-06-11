#pragma once
#include "Component.h"

class Model;
class Shader;
class Material;

class ModelRenderer :
    public Component
{
    using Super = Component;


public:
    ModelRenderer(shared_ptr<Shader> _shader);
    virtual ~ModelRenderer();

    virtual void Update() override;

    void SetModel(shared_ptr<Model> _model);
    void SetPass(uint8 _pass) { m_pass = _pass; }

private:
    shared_ptr<Shader>  m_shader;
    uint8               m_pass = 0;
    shared_ptr<Model>   m_model;
};

