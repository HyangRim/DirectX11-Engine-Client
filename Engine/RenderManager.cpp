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


	m_boneBuffer = make_shared<ConstantBuffer<BoneDesc>>();
	m_boneBuffer->Create();
	m_boneEffectBuffer = m_shader->GetConstantBuffer("BoneBuffer");

	m_keyframeBuffer = make_shared<ConstantBuffer<KeyframeDesc>>();
	m_keyframeBuffer->Create();
	m_keyframeEffectBuffer = m_shader->GetConstantBuffer("KeyframeBuffer");
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

void RenderManager::PushBoneData(const BoneDesc& _desc)
{
	m_boneDesc = _desc;
	m_boneBuffer->CopyData(m_boneDesc);
	m_boneEffectBuffer->SetConstantBuffer(m_boneBuffer->GetComPtr().Get());
}

void RenderManager::PushKeyframeData(const KeyframeDesc& _desc)
{
	m_keyframeDesc = _desc;
	m_keyframeBuffer->CopyData(m_keyframeDesc);
	m_keyframeEffectBuffer->SetConstantBuffer(m_keyframeBuffer->GetComPtr().Get());
}
