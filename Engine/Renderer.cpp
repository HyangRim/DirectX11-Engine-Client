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

void Renderer::Render()
{

	if (m_material == nullptr)
		return;

	const auto& shader = m_material->GetShader();
	if (shader == nullptr)
		return;

	shader->PushGlobalData(Camera::s_MatView, Camera::s_MatProjection);

	auto lightObj = CURSCENE->GetLight();

	if (lightObj) {
		shader->PushLightData(lightObj->GetLight()->GetLightDesc());
	}
}
