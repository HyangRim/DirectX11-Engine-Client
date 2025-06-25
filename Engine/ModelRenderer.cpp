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

	//ī�޶�, �� ����� Render����. 
	if (Super::Render(_isShadowTech) == false)
		return;

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

		m_shader->DrawIndexedInstanced(GET_TECH(_isShadowTech), m_pass, mesh->m_indexBuffer->GetCount(), _buffer->GetCount());
	}
}

InstanceID ModelRenderer::GetInstanceID()
{
	//�����͸� ���� ID�߱�.
	return make_pair((uint64)m_model.get(), (uint64)m_shader.get());
}

void ModelRenderer::SetMaterial(shared_ptr<Material> _material)
{
	assert(false, "ModelRenderer::SetMaterial is not supported. Use SetModel instead.");
}
