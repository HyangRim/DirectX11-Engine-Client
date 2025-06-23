#include "pch.h"
#include "Material.h"

Material::Material() : Super(ResourceType::Material)
{
}

Material::~Material()
{
}

void Material::SetShader(shared_ptr<Shader> _shader)
{
	m_shader = _shader;

	m_diffuseEffectBuffer = m_shader->GetSRV("DiffuseMap");
	m_normalEffectBuffer = m_shader->GetSRV("NormalMap");
	m_specularEffectBuffer = m_shader->GetSRV("SpecularMap");
	m_randomEffectBuffer = m_shader->GetSRV("RandomMap");
}

void Material::Update()
{
	if (m_shader == nullptr)
		return;

	//RENDER->PushMaterialData(m_desc);
	m_shader->PushMaterialData(m_desc);
	if (m_diffuseMap)
		m_diffuseEffectBuffer->SetResource(m_diffuseMap->GetComPtr().Get());

	if (m_normalMap)
		m_normalEffectBuffer->SetResource(m_normalMap->GetComPtr().Get());

	if (m_specularMap)
		m_specularEffectBuffer->SetResource(m_specularMap->GetComPtr().Get());

	if (m_randomMap)
		m_randomEffectBuffer->SetResource(m_randomMap->GetComPtr().Get());
}

shared_ptr<Material> Material::Clone()
{
	shared_ptr<Material> material = make_shared<Material>();

	material->m_desc = m_desc;
	material->m_shader = m_shader;
	material->m_diffuseMap = m_diffuseMap;
	material->m_normalMap = m_normalMap;
	material->m_specularMap = m_specularMap;
	material->m_diffuseEffectBuffer = m_diffuseEffectBuffer;
	material->m_normalEffectBuffer = m_normalEffectBuffer;
	material->m_specularEffectBuffer = m_specularEffectBuffer;

	return material;
}
