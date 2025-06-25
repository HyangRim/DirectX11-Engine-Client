#include "pch.h"
#include "ModelAnimator.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "ModelMesh.h"
#include "ModelAnimation.h"
#include "Camera.h"
#include "Light.h"

ModelAnimator::ModelAnimator(shared_ptr<Shader> _shader)
	: Super(ComponentType::Animator), m_shader(_shader)
{
	m_tweenDesc.m_next.m_animIndex = rand() % 3;
	m_tweenDesc.m_tweenSumTime += rand() % 100;
}

ModelAnimator::~ModelAnimator()
{
}

void ModelAnimator::Update()
{
}

void ModelAnimator::UpdateTweenData()
{
	TweenDesc& desc = m_tweenDesc;

	desc.m_curr.m_sumTime += DT;
	//���� �ִϸ��̼� ����

	{
		shared_ptr<ModelAnimation> currentAnim = m_model->GetAnimationByIndex(desc.m_curr.m_animIndex);
		if (currentAnim)
		{
			float timePerFrame = 1 / (currentAnim->m_frameRate * desc.m_curr.m_speed);
			if (desc.m_curr.m_sumTime >= timePerFrame)
			{
				desc.m_curr.m_sumTime = 0;
				desc.m_curr.m_currFrame = (desc.m_curr.m_currFrame + 1) % currentAnim->m_frameCount;
				desc.m_curr.m_nextFrame = (desc.m_curr.m_currFrame + 1) % currentAnim->m_frameCount;
			}

			desc.m_curr.m_ratio = (desc.m_curr.m_sumTime / timePerFrame);
		}

	}

	//���� �ִϸ��̼��� ����Ǿ� �ִٸ�.
	if (desc.m_next.m_animIndex >= 0) {
		desc.m_tweenSumTime += DT;
		desc.m_tweenRatio = desc.m_tweenSumTime / desc.m_tweenDuration;

		if (desc.m_tweenRatio >= 1.f) {
			//�ִϸ��̼� ��ü ��.
			desc.m_curr = desc.m_next;
			desc.ClearNextAnim();
		}
		else {
			//�ִϸ��̼� ��ü ��. 
			shared_ptr<ModelAnimation> nextAnim = m_model->GetAnimationByIndex(desc.m_next.m_animIndex);
			desc.m_next.m_sumTime += DT;

			float timePerFrame = 1.f / (nextAnim->m_frameRate * desc.m_next.m_speed);


			if (desc.m_next.m_ratio >= 1.f)
			{
				desc.m_next.m_sumTime = 0;

				desc.m_next.m_currFrame = (desc.m_next.m_currFrame + 1) % nextAnim->m_frameCount;
				desc.m_next.m_nextFrame = (desc.m_next.m_currFrame + 1) % nextAnim->m_frameCount;
			}
			desc.m_next.m_ratio = desc.m_next.m_sumTime / timePerFrame;

		}
	}



}

void ModelAnimator::SetModel(shared_ptr<Model> _model)
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

void ModelAnimator::RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer, bool _isShadowTech)
{
	if (m_model == nullptr)
		return;

	if (Super::Render(_isShadowTech) == false)
		return;


	//TODO(Animation)
	//TransformMap�� ���� ����. 
	if (m_texture == nullptr)
		CreateTexture();


	//SRV�� ���� ���� ����. 
	m_shader->GetSRV("TransformMap")->SetResource(m_srv.Get());

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
	m_shader->PushBoneData(boneDesc);
	//RENDER->PushBoneData(boneDesc);

	//Mesh���� ���. 
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

		_buffer->PushData();
		m_shader->DrawIndexedInstanced(GET_TECH(_isShadowTech), m_pass, mesh->m_indexBuffer->GetCount(), _buffer->GetCount());
	}
}

InstanceID ModelAnimator::GetInstanceID()
{
	return make_pair((uint64)m_model.get(), (uint64)m_shader.get());
}

void ModelAnimator::CreateTexture()
{
	if (m_model->GetAnimationCount() == 0)
		return;

	m_animTransform.resize(m_model->GetAnimationCount());
	for (uint32 idx = 0; idx < m_model->GetAnimationCount(); ++idx) {
		CreateAnimationTransform(idx);
	}

	// Creature Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_BONE_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = m_model->GetAnimationCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16����Ʈ(�ִ�)
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		const uint32 dataSize = MAX_BONE_TRANSFORMS * sizeof(Matrix);
		const uint32 pageSize = dataSize * MAX_MODEL_KEYFRAMES;

		//C���� ����ϴ� �Ҵ�. 
		//������ ȣ���ϴ��� ������ ����. �츮�� ������ ȣ�� �ʿ� ���. 
		void* mallocPtr = ::malloc(pageSize * m_model->GetAnimationCount());

		// ����ȭ�� �����͸� �����Ѵ�.
		for (uint32 c = 0; c < m_model->GetAnimationCount(); c++)
		{
			uint32 startOffset = c * pageSize;

			BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;

			for (uint32 f = 0; f < MAX_MODEL_KEYFRAMES; f++)
			{
				void* ptr = pageStartPtr + dataSize * f;
				::memcpy(ptr, m_animTransform[c].transforms[f].data(), dataSize);
			}
		}

		// ���ҽ� �����
		vector<D3D11_SUBRESOURCE_DATA> subResources(m_model->GetAnimationCount());

		for (uint32 c = 0; c < m_model->GetAnimationCount(); c++)
		{
			void* ptr = (BYTE*)mallocPtr + c * pageSize;
			subResources[c].pSysMem = ptr;
			subResources[c].SysMemPitch = dataSize;
			subResources[c].SysMemSlicePitch = pageSize;
		}

		HRESULT hr = DEVICE->CreateTexture2D(&desc, subResources.data(), m_texture.GetAddressOf());
		CHECK(hr);

		::free(mallocPtr);
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = m_model->GetAnimationCount();

		HRESULT hr = DEVICE->CreateShaderResourceView(m_texture.Get(), &desc, m_srv.GetAddressOf());
		CHECK(hr);
	}
	//�ᱹ ���̴� ���ҽ� �䰡 �츮�� ���� �⵿��ϸ� ��� �ְ�.
	//�װ� GPU�� �Ѱ��༭, �װ� ������� ���̴��� ����ؼ� ��ȯ�Ѵ�. 

}

void ModelAnimator::CreateAnimationTransform(uint32 _index)
{

	//ĳ�� �뵵. 
	vector<Matrix> tempAnimBoneTransforms(MAX_BONE_TRANSFORMS, Matrix::Identity);
	
	shared_ptr<ModelAnimation> animation = m_model->GetAnimationByIndex(_index);

	for (uint32 frame = 0; frame < animation->m_frameCount; ++frame) {
		for (uint32 bone = 0; bone < m_model->GetBoneCount(); ++bone) {
			shared_ptr<ModelBone> tbone = m_model->GetBoneByIndex(bone);
			Matrix matAnimation;


			shared_ptr<ModelKeyframe> tframe = animation->GetKeyframe(tbone->m_name);
			if (tframe != nullptr) {
				ModelKeyframeData& data = tframe->m_transforms[frame];


				//Ư�� ���� Ư�� �������� SRT���ϱ�. 
				Matrix S, R, T;
				S = Matrix::CreateScale(data.m_scale.x, data.m_scale.y, data.m_scale.z);
				R = Matrix::CreateFromQuaternion(data.m_rotation);
				T = Matrix::CreateTranslation(data.m_translation.x, data.m_translation.y, data.m_translation.z);

				//�̰� � �ǹ̸� ������ �ִ°�? 
				//���� �θ���� Relative��. 
				matAnimation = S * R * T;
			}
			else {
				matAnimation = Matrix::Identity;
			}

			// T����� ���õ� ��. 
			// ���� -> Global�� m_transform�� �־������.
			Matrix toRootMatrix = tbone->m_transform;
			//�ڽ��� ������ �������� �� ��ǥ��� �ٲٴ� ��. 
			Matrix invGlobal = toRootMatrix.Invert();

			int32 parentIndex = tbone->m_parentIndex;

			//Anim�� ���õ� ��. 
			Matrix matParent = Matrix::Identity;
			if (parentIndex >= 0)
				matParent = tempAnimBoneTransforms[parentIndex];
			tempAnimBoneTransforms[bone] = matAnimation * matParent;

			// ���. 
			//T ���� ���� Global ��ǥ�谡 �ִµ�, ��ȭ�ؼ� ������ �������� �� ��� ��ǥ��� �ٲ� ������. 
			//�װ� �ٽ� ���ư���, Global�� �ٲ۴�. �ڽ� -> �θ�(Root����, Global) -> �ڽ�. 

			//�������ٰ� �� �κ��� ���ϸ�, �ش� ��ġ�� �ش� ���밡 �����δ�. 
			// �ִ� 4���� ���� ���� �־, �׷� �� �����ؼ� ���̴����� ��ǥ ��ȯ�Ѵ�. 

			m_animTransform[_index].transforms[frame][bone] = invGlobal * tempAnimBoneTransforms[bone];
		}
	}
}

shared_ptr<Shader> ModelAnimator::GetShader() {
	return m_material->GetShader();
}