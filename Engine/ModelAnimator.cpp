#include "pch.h"
#include "ModelAnimator.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "ModelMesh.h"
#include "ModelAnimation.h"

ModelAnimator::ModelAnimator(shared_ptr<Shader> _shader)
	: Super(ComponentType::Animator), m_shader(_shader)
{
}

ModelAnimator::~ModelAnimator()
{
}

void ModelAnimator::Update()
{
	if (m_model == nullptr)
		return;

	//TODO(Animation)
	if (m_texture == nullptr)
		CreateTexture();


	m_keyframeDesc.m_sumTime += DT;
	shared_ptr<ModelAnimation> current = m_model->GetAnimationByIndex(m_keyframeDesc.animIndex);

	if (current) {
		float timePerFrame = 1 / (current->m_frameRate * m_keyframeDesc.m_speed);
		if (m_keyframeDesc.m_sumTime >= timePerFrame) {
			m_keyframeDesc.m_sumTime = 0.f;
			m_keyframeDesc.currFrame = (m_keyframeDesc.currFrame + 1) % current->m_frameCount;
			m_keyframeDesc.m_nextFrame = (m_keyframeDesc.currFrame + 1) % current->m_frameCount;
		}

		m_keyframeDesc.m_ratio = (m_keyframeDesc.m_sumTime / timePerFrame);
	}

	//Anim Update
	ImGui::InputInt("AnimIndex", &m_keyframeDesc.animIndex);
	m_keyframeDesc.animIndex %= m_model->GetAnimationCount();
	ImGui::InputFloat("Speed", &m_keyframeDesc.m_speed, 0.5f, 4.f);
	//m_keyframeDesc.currFrame %= m_model->GetAnimationByIndex(m_keyframeDesc.animIndex)->m_frameCount;

	//애니메이션 현재 프레임 정보
	RENDER->PushKeyframeData(m_keyframeDesc);

	//SRV를 통해 정보 전달. 
	m_shader->GetSRV("TransformMap")->SetResource(m_srv.Get());

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
	}
}

void ModelAnimator::SetModel(shared_ptr<Model> _model)
{
	m_model = _model;

	const auto& materials = m_model->GetMaterials();
	for (auto& material : materials)
	{
		material->SetShader(m_shader);
	}
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
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트(최댓값)
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		const uint32 dataSize = MAX_BONE_TRANSFORMS * sizeof(Matrix);
		const uint32 pageSize = dataSize * MAX_MODEL_KEYFRAMES;

		//C에서 사용하는 할당. 
		//생성자 호출하느냐 마느냐 차이. 우리는 생성자 호출 필요 없어서. 
		void* mallocPtr = ::malloc(pageSize * m_model->GetAnimationCount());

		// 파편화된 데이터를 조립한다.
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

		// 리소스 만들기
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
	//결국 셰이더 리소스 뷰가 우리의 저런 잡동사니를 들고 있고.
	//그걸 GPU에 넘겨줘서, 그걸 기반으로 셰이더가 계산해서 변환한다. 

}

void ModelAnimator::CreateAnimationTransform(uint32 _index)
{

	//캐싱 용도. 
	vector<Matrix> tempAnimBoneTransforms(MAX_BONE_TRANSFORMS, Matrix::Identity);
	
	shared_ptr<ModelAnimation> animation = m_model->GetAnimationByIndex(_index);

	for (uint32 frame = 0; frame < animation->m_frameCount; ++frame) {
		for (uint32 bone = 0; bone < m_model->GetBoneCount(); ++bone) {
			shared_ptr<ModelBone> tbone = m_model->GetBoneByIndex(bone);
			Matrix matAnimation;


			shared_ptr<ModelKeyframe> tframe = animation->GetKeyframe(tbone->m_name);
			if (tframe != nullptr) {
				ModelKeyframeData& data = tframe->m_transforms[frame];


				//특정 본의 특정 프레임의 SRT구하기. 
				Matrix S, R, T;
				S = Matrix::CreateScale(data.m_scale.x, data.m_scale.y, data.m_scale.z);
				R = Matrix::CreateFromQuaternion(data.m_rotation);
				T = Matrix::CreateTranslation(data.m_translation.x, data.m_translation.y, data.m_translation.z);

				//이게 어떤 의미를 가지고 있는가? 
				//상위 부모로의 Relative임. 
				matAnimation = S * R * T;
			}
			else {
				matAnimation = Matrix::Identity;
			}

			// T포즈와 관련된 것. 
			// 현재 -> Global을 m_transform에 넣어놓았음.
			Matrix toRootMatrix = tbone->m_transform;
			//자신의 관절을 기준으로 한 좌표계로 바꾸는 것. 
			Matrix invGlobal = toRootMatrix.Invert();

			int32 parentIndex = tbone->m_parentIndex;

			//Anim과 관련된 것. 
			Matrix matParent = Matrix::Identity;
			if (parentIndex >= 0)
				matParent = tempAnimBoneTransforms[parentIndex];
			tempAnimBoneTransforms[bone] = matAnimation * matParent;

			// 결론. 
			//T 포즈 기준 Global 좌표계가 있는데, 변화해서 관절을 기준으로 한 상대 좌표계로 바꾼 다음에. 
			//그걸 다시 돌아가서, Global로 바꾼다. 자신 -> 부모(Root기준, Global) -> 자신. 

			//정점에다가 이 부분을 곱하면, 해당 위치로 해당 뼈대가 움직인다. 
			// 최대 4개의 뼈에 묶여 있어서, 그런 걸 생각해서 쉐이더에서 좌표 변환한다. 

			m_animTransform[_index].transforms[frame][bone] = invGlobal * tempAnimBoneTransforms[bone];
		}
	}
}
