#include "pch.h"
#include "ModelRenderer.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "ModelMesh.h"
#include "Camera.h"
#include "Light.h"

ModelRenderer::ModelRenderer(shared_ptr<Shader> _shader) : Super(ComponentType::ModelRenderer), m_shader(_shader)
{
}

ModelRenderer::~ModelRenderer()
{
}

//void ModelRenderer::Update()
//{
//	
//	if (m_model == nullptr)
//		return;
//	
//	auto world = GetTransform()->GetWorldMatrix();
//	RENDER->PushTransformData(TransformDesc{ world });
//	const auto& meshes = m_model->GetMeshes();
//	for (auto& mesh : meshes)
//	{
//		if (mesh->m_material)
//			mesh->m_material->Update();
//		uint32 stride = mesh->m_vertexBuffer->GetStride();
//		uint32 offset = mesh->m_vertexBuffer->GetOffset();
//		DC->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
//		DC->IASetIndexBuffer(mesh->m_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//		m_shader->DrawIndexed(0, m_pass, mesh->m_indexBuffer->GetCount(), 0, 0);
//	}
//}
/*
void ModelRenderer::Update()
{
	if (m_model == nullptr)
		return;

	// Bones
	BoneDesc boneDesc;

	//�� ���� ����, �� ������ŭ ������ֱ�. 
	//�׸���, �� ������ ���� GPU�� �о�־��ֱ�. 
	const uint32 boneCount = m_model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->m_transform;
	}
	RENDER->PushBoneData(boneDesc);

	// Transform
	auto world = GetTransform()->GetWorldMatrix();
	RENDER->PushTransformData(TransformDesc{ world });


	//Mesh���� ���. 
	const auto& meshes = m_model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->m_material)
			mesh->m_material->Update();

		// BoneIndex
		//�װ� �� ��° Bone���� �־��ֱ�. 
		m_shader->GetScalar("BoneIndex")->SetInt(mesh->m_boneIndex);

		uint32 stride = mesh->m_vertexBuffer->GetStride();
		uint32 offset = mesh->m_vertexBuffer->GetOffset();

		DC->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
		DC->IASetIndexBuffer(mesh->m_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

		m_shader->DrawIndexed(0, m_pass, mesh->m_indexBuffer->GetCount(), 0, 0);

		//m_shader->DrawIndexedInstanced
	}
}
*/


void ModelRenderer::SetModel(shared_ptr<Model> _model)
{
	m_model = _model;

	const auto& materials = m_model->GetMaterials();
	for (auto& material : materials)
	{
		material->SetShader(m_shader);
	}
}

void ModelRenderer::RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer)
{
	if (m_model == nullptr)
		return;


	//GlobalData
	m_shader->PushGlobalData(Camera::s_MatView, Camera::s_MatProjection);

	//Light
	auto lightObj = SCENE->GetCurScene()->GetLight();
	if (lightObj)
		m_shader->PushLightData(lightObj->GetLight()->GetLightDesc());

	// Bones
	BoneDesc boneDesc;

	//�� ���� ����, �� ������ŭ ������ֱ�. 
	//�׸���, �� ������ ���� GPU�� �о�־��ֱ�. 

	//�ν��Ͻ��� ����, �� ������Ʈ�� 
	const uint32 boneCount = m_model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->m_transform;
	}
	m_shader->PushBoneData(boneDesc);
	//RENDER->PushBoneData(boneDesc);

	//Mesh���� ���. 

	//���� ���� ����ȭ �Ǿ� ������(���� 10��)�� ��, ��ο��� 10�� �ؾ���...
	const auto& meshes = m_model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->m_material)
			mesh->m_material->Update();

		// BoneIndex
		//�װ� �� ��° Bone���� �־��ֱ�. 
		m_shader->GetScalar("BoneIndex")->SetInt(mesh->m_boneIndex);


		mesh->m_vertexBuffer->PushData();
		mesh->m_indexBuffer->PushData();


		//���̴����� ���⼭ �� ������Ʈ�� WORLDPOSITION�� �־��ִ� ��. 
		_buffer->PushData();

		m_shader->DrawIndexedInstanced(0, m_pass, mesh->m_indexBuffer->GetCount(), _buffer->GetCount());
	}
}

InstanceID ModelRenderer::GetInstanceID()
{
	//�����͸� ���� ID�߱�.
	return make_pair((uint64)m_model.get(), (uint64)m_shader.get());
}
