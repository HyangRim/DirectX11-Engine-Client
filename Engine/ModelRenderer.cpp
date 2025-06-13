#include "pch.h"
#include "ModelRenderer.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "ModelMesh.h"

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

void ModelRenderer::Update()
{
	if (m_model == nullptr)
		return;

	// Bones
	BoneDesc boneDesc;

	//본 갯수 새고, 그 갯수만큼 만들어주기. 
	//그리고, 그 정보에 대해 GPU에 밀어넣어주기. 
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


	//Mesh마다 출력. 
	const auto& meshes = m_model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->m_material)
			mesh->m_material->Update();

		// BoneIndex
		//그게 몇 번째 Bone인지 넣어주기. 
		m_shader->GetScalar("BoneIndex")->SetInt(mesh->m_boneIndex);

		uint32 stride = mesh->m_vertexBuffer->GetStride();
		uint32 offset = mesh->m_vertexBuffer->GetOffset();

		DC->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
		DC->IASetIndexBuffer(mesh->m_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

		m_shader->DrawIndexed(0, m_pass, mesh->m_indexBuffer->GetCount(), 0, 0);

		//m_shader->DrawIndexedInstanced
	}
}



void ModelRenderer::SetModel(shared_ptr<Model> _model)
{
	m_model = _model;

	const auto& materials = m_model->GetMaterials();
	for (auto& material : materials)
	{
		material->SetShader(m_shader);
	}
}
