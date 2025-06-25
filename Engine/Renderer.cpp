#include "pch.h"
#include "Renderer.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

Renderer::Renderer(ComponentType _componentType) : Super(_componentType)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Render(bool _isShadowTech)
{
	if (m_material == nullptr)
		return false;

	if (m_material->GetCastShadow() == false && _isShadowTech == true) {
		return false;
	}

	InnerRender(_isShadowTech);
	return true;
}

void Renderer::InnerRender(bool _isShadowTech)
{
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	const auto& shader = m_material->GetShader();
	if (shader == nullptr)
		return;

	if (_isShadowTech == false) {
		m_material->Update();
		
		//Light
		auto lightObj = CURSCENE->GetLight();

		if (lightObj) {
			shader->PushLightData(lightObj->GetLight()->GetLightDesc());
		}
		shader->PushGlobalData(Camera::s_MatView, Camera::s_MatProjection);
		shader->PushShadowData(Light::s_ShadowTransform);
	}
	else {
		shader->PushGlobalData(Light::s_MatView, Light::s_MatProjection);
	}

	shader->PushTransformData(TransformDesc{ GetTransform()->GetWorldMatrix() });
}
