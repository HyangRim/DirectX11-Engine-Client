#include "pch.h"
#include "ModelRenderer.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "ModelMesh.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"

ModelRenderer::ModelRenderer(shared_ptr<Shader> _shader) : Super(ComponentType::ModelRenderer), m_shader(_shader)
{
}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::SetModel(shared_ptr<Model> _model)
{
	m_model = _model;

	const auto& materials = m_model->GetMaterials();
	for (auto& material : materials)
	{
		material->SetShader(m_shader);
		m_material = material;
		break;
	}
}

void ModelRenderer::RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer, bool _isShadowTech)
{
	if (m_model == nullptr)
		return;

	//카메라, 빛 계산은 Render에서. 
	if (Super::Render(_isShadowTech) == false)
		return;

	// Bones
	BoneDesc boneDesc;

	//본 갯수 새고, 그 갯수만큼 만들어주기. 
	//그리고, 그 정보에 대해 GPU에 밀어넣어주기. 

	//인스턴싱을 위해, 각 오브젝트는 
	const uint32 boneCount = m_model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->m_transform;
	}
	m_shader->PushBoneData(boneDesc);
	//RENDER->PushBoneData(boneDesc);

	//Mesh마다 출력. 

	//내부 모델이 계층화 되어 있으면(파츠 10개)일 때, 드로우콜 10번 해야함...
	const auto& meshes = m_model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->m_material)
			mesh->m_material->Update();

		// BoneIndex
		//그게 몇 번째 Bone인지 넣어주기. 
		m_shader->GetScalar("BoneIndex")->SetInt(mesh->m_boneIndex);


		mesh->m_vertexBuffer->PushData();
		mesh->m_indexBuffer->PushData();


		//쉐이더한테 여기서 각 오브젝트별 WORLDPOSITION을 넣어주는 중. 
		_buffer->PushData();

		m_shader->DrawIndexedInstanced(GET_TECH(_isShadowTech), m_pass, mesh->m_indexBuffer->GetCount(), _buffer->GetCount());
	}
}

InstanceID ModelRenderer::GetInstanceID()
{
	//포인터를 통한 ID발급.
	return make_pair((uint64)m_model.get(), (uint64)m_shader.get());
}

void ModelRenderer::SetMaterial(shared_ptr<Material> _material)
{
	assert(false, "ModelRenderer::SetMaterial is not supported. Use SetModel instead.");
}
