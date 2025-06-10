#include "pch.h"
#include "RenderManager.h"
#include "Camera.h"

void RenderManager::init(shared_ptr<Shader> _shader)
{
	m_shader = _shader;

	m_globalBuffer = make_shared<ConstantBuffer<GlobalDesc>>();
	m_globalBuffer->Create();
	m_globalEffectBuffer = m_shader->GetConstantBuffer("GlobalBuffer");

	m_transformBuffer = make_shared<ConstantBuffer<TransformDesc>>();
	m_transformBuffer->Create();
	m_transformEffectBuffer = m_shader->GetConstantBuffer("TransformBuffer");


	m_lightBuffer = make_shared<ConstantBuffer<LightDesc>>();
	m_lightBuffer->Create();
	m_lightEffectBuffer = m_shader->GetConstantBuffer("LightBuffer");

	m_materialBuffer = make_shared<ConstantBuffer<MaterialDesc>>();
	m_materialBuffer->Create();
	m_materialEffectBuffer = m_shader->GetConstantBuffer("MaterialBuffer");
}

void RenderManager::Update()
{
	PushGlobalData(Camera::s_MatView, Camera::s_MatProjection);
}


//CPU -> GPU로 밀어넣는 연산. 
void RenderManager::PushGlobalData(const Matrix& _view, const Matrix& _projection)
{
	m_globalDesc.P = _projection;
	m_globalDesc.V = _view;
	m_globalDesc.VP = _view * _projection;
	m_globalDesc.Vinv = _view.Invert();

	m_globalBuffer->CopyData(m_globalDesc);
	m_globalEffectBuffer->SetConstantBuffer(m_globalBuffer->GetComPtr().Get());

}

void RenderManager::PushTransformData(const TransformDesc& _desc)
{
	m_transformDesc = _desc;

	m_transformBuffer->CopyData(m_transformDesc);
	m_transformEffectBuffer->SetConstantBuffer(m_transformBuffer->GetComPtr().Get());
}

void RenderManager::PushLightData(const LightDesc& _desc)
{
	m_lightDesc = _desc;
	m_lightBuffer->CopyData(m_lightDesc);
	m_lightEffectBuffer->SetConstantBuffer(m_lightBuffer->GetComPtr().Get());
}

void RenderManager::PushMaterialData(const MaterialDesc& _desc)
{
	m_materialDesc = _desc;
	m_materialBuffer->CopyData(m_materialDesc);
	m_materialEffectBuffer->SetConstantBuffer(m_materialBuffer->GetComPtr().Get());
}
