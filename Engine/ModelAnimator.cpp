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
    //m_tweenDesc.m_next.m_animIndex = rand() % 3;
    //m_tweenDesc.m_tweenSumTime += rand() % 100;
    m_loopSettings[AnimationState::Wait] = true;
    m_loopSettings[AnimationState::Run] = true;
    m_loopSettings[AnimationState::Skill] = false;  // 스킬은 한 번만 실행
    m_loopSettings[AnimationState::BaseAttack] = false;

    // 상태를 태그로 매핑 (초기화 시 설정)
    m_stateToTag[AnimationState::Wait] = L"Wait";
    m_stateToTag[AnimationState::Run] = L"Run";
    m_stateToTag[AnimationState::BaseAttack] = L"BaseAttack";
    m_stateToTag[AnimationState::Skill] = L"Skill";
}

ModelAnimator::~ModelAnimator()
{
    m_srv.Reset();
    m_texture.Reset();
}

void ModelAnimator::Update()
{
}

void ModelAnimator::UpdateTweenData()
{
    // 애니메이션 상태 업데이트 (키 입력 처리 포함)
    UpdateAnimationState();

    // 스킬 쿨다운 처리
    if (m_skillCooldown > 0.0f)
    {
        m_skillCooldown -= DT;
        if (m_skillCooldown <= 0.0f)
        {
            m_isSkillActive = false;
            // 스킬 완료 후 Wait로 전환
            TransitionToAnimation(AnimationState::Wait);
        }
    }

    TweenDesc& desc = m_tweenDesc;

    desc.m_curr.m_sumTime += DT;
    //현재 애니메이션 관련

    {
        // 인덱스 기반에서 태그 기반으로 변경
        wstring currentTag = m_indexToTag[desc.m_curr.m_animIndex];
        shared_ptr<ModelAnimation> currentAnim = m_model->GetAnimationByTag(currentTag);
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

    //다음 애니메이션이 예약되어 있다면.
    if (desc.m_next.m_animIndex >= 0) {
        desc.m_tweenSumTime += DT;
        desc.m_tweenRatio = desc.m_tweenSumTime / desc.m_tweenDuration;

        if (desc.m_tweenRatio >= 1.f) {
            //애니메이션 교체 끝.
            desc.m_curr = desc.m_next;
            desc.ClearNextAnim();
        }
        else {
            //애니메이션 교체 중. 
            wstring nextTag = m_indexToTag[desc.m_next.m_animIndex];
            shared_ptr<ModelAnimation> nextAnim = m_model->GetAnimationByTag(nextTag);
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

    // 태그와 인덱스 매핑 생성
    m_tagToIndex.clear();
    m_indexToTag.clear();

    uint32 index = 0;
    for (const auto& pair : m_model->GetAnimations())
    {
        m_tagToIndex[pair.first] = index;
        m_indexToTag.push_back(pair.first);
        index++;
    }
}

void ModelAnimator::RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer, bool _isShadowTech)
{
    if (m_model == nullptr)
        return;

    if (Super::Render(_isShadowTech) == false)
        return;

    //TODO(Animation)
    //TransformMap은 뼈대 정보. 
    if (m_texture == nullptr)
        CreateTexture();

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
    m_shader->PushBoneData(boneDesc);

    //Mesh마다 출력. 
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

    uint32 actualBoneCount = m_model->GetBoneCount();
    uint32 maxFrameCount = 0;

    // 태그 기반으로 변경
    for (const auto& pair : m_model->GetAnimations())
    {
        maxFrameCount = max(maxFrameCount, pair.second->m_frameCount);
        CreateAnimationTransform(pair.first);
    }

    // Create Texture
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
        desc.Width = MAX_BONE_TRANSFORMS * 4;
        desc.Height = maxFrameCount;
        desc.ArraySize = m_model->GetAnimationCount();
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트(최댓값)
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;

        const uint32 dataSize = MAX_BONE_TRANSFORMS * sizeof(Matrix);
        const uint32 pageSize = dataSize * maxFrameCount;

        void* mallocPtr = ::malloc(pageSize * m_model->GetAnimationCount());

        // 파편화된 데이터를 조립한다.
        uint32 animIndex = 0;
        for (const auto& pair : m_model->GetAnimations())
        {
            uint32 startOffset = animIndex * pageSize;
            BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;

            for (uint32 f = 0; f < maxFrameCount; f++)
            {
                void* ptr = pageStartPtr + dataSize * f;
                ::memcpy(ptr, m_animTransform[pair.first].transforms[f].data(), dataSize);
            }
            animIndex++;
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
}

void ModelAnimator::CreateAnimationTransform(const wstring& _tag)
{
    //캐싱 용도. 
    vector<Matrix> tempAnimBoneTransforms(MAX_BONE_TRANSFORMS, Matrix::Identity);

    shared_ptr<ModelAnimation> animation = m_model->GetAnimationByTag(_tag);
    if (!animation)
        return;

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

            //수정사항
            m_animTransform[_tag].transforms[frame][bone] = tbone->m_offsetMatrix * tempAnimBoneTransforms[bone];
        }
    }
}

// 기존 인덱스 기반 메서드 주석처리
//void ModelAnimator::CreateAnimationTransform(uint32 _index)
//{
//    //캐싱 용도. 
//    vector<Matrix> tempAnimBoneTransforms(MAX_BONE_TRANSFORMS, Matrix::Identity);
//
//    shared_ptr<ModelAnimation> animation = m_model->GetAnimationByIndex(_index);
//
//    for (uint32 frame = 0; frame < animation->m_frameCount; ++frame) {
//        for (uint32 bone = 0; bone < m_model->GetBoneCount(); ++bone) {
//            shared_ptr<ModelBone> tbone = m_model->GetBoneByIndex(bone);
//            Matrix matAnimation;
//
//            shared_ptr<ModelKeyframe> tframe = animation->GetKeyframe(tbone->m_name);
//            if (tframe != nullptr) {
//                ModelKeyframeData& data = tframe->m_transforms[frame];
//
//                //특정 본의 특정 프레임의 SRT구하기. 
//                Matrix S, R, T;
//                S = Matrix::CreateScale(data.m_scale.x, data.m_scale.y, data.m_scale.z);
//                R = Matrix::CreateFromQuaternion(data.m_rotation);
//                T = Matrix::CreateTranslation(data.m_translation.x, data.m_translation.y, data.m_translation.z);
//
//                //이게 어떤 의미를 가지고 있는가? 
//                //상위 부모로의 Relative임. 
//                matAnimation = S * R * T;
//            }
//            else {
//                matAnimation = Matrix::Identity;
//            }
//
//            // T포즈와 관련된 것. 
//            // 현재 -> Global을 m_transform에 넣어놓았음.
//            Matrix toRootMatrix = tbone->m_transform;
//            //자신의 관절을 기준으로 한 좌표계로 바꾸는 것. 
//            Matrix invGlobal = toRootMatrix.Invert();
//
//            int32 parentIndex = tbone->m_parentIndex;
//
//            //Anim과 관련된 것. 
//            Matrix matParent = Matrix::Identity;
//            if (parentIndex >= 0)
//                matParent = tempAnimBoneTransforms[parentIndex];
//            tempAnimBoneTransforms[bone] = matAnimation * matParent;
//
//            //수정사항
//            m_animTransform[_index].transforms[frame][bone] = tbone->m_offsetMatrix * tempAnimBoneTransforms[bone];
//        }
//    }
//}

shared_ptr<Shader> ModelAnimator::GetShader()
{
    return m_material->GetShader();
}

void ModelAnimator::SetAnimation(uint32 _animIndex, bool _immediate)
{
    if (_animIndex >= m_model->GetAnimationCount())
        return;

    if (_immediate) {
        // 즉시 애니메이션 변경
        m_tweenDesc.m_curr.m_animIndex = _animIndex;
        m_tweenDesc.m_curr.m_currFrame = 0;
        m_tweenDesc.m_curr.m_nextFrame = 1;
        m_tweenDesc.m_curr.m_sumTime = 0.f;
        m_tweenDesc.m_curr.m_ratio = 0.f;
        m_tweenDesc.ClearNextAnim();
    }
    else {
        SetNextAnimation(_animIndex);
    }
}

void ModelAnimator::SetAnimationByTag(const wstring& _tag, bool _immediate)
{
    auto it = m_tagToIndex.find(_tag);
    if (it == m_tagToIndex.end())
        return;

    SetAnimation(it->second, _immediate);
}

void ModelAnimator::SetNextAnimation(uint32 _animIndex, bool _tweenDuration)
{
    if (_animIndex >= m_model->GetAnimationCount())
        return;

    m_tweenDesc.m_next.m_animIndex = _animIndex;
    m_tweenDesc.m_next.m_currFrame = 0;
    m_tweenDesc.m_next.m_nextFrame = 1;
    m_tweenDesc.m_next.m_sumTime = 0.f;
    m_tweenDesc.m_next.m_ratio = 0.f;
    m_tweenDesc.m_tweenDuration = _tweenDuration;
    m_tweenDesc.m_tweenSumTime = 0.f;
    m_tweenDesc.m_tweenRatio = 0.f;
}

void ModelAnimator::SetNextAnimationByTag(const wstring& _tag, bool _tweenDuration)
{
    auto it = m_tagToIndex.find(_tag);
    if (it == m_tagToIndex.end())
        return;

    SetNextAnimation(it->second, _tweenDuration);
}

void ModelAnimator::SetAnimationSpeed(float _speed)
{
    m_tweenDesc.m_curr.m_speed = _speed;
    if (m_tweenDesc.m_next.m_animIndex >= 0)
        m_tweenDesc.m_next.m_speed = _speed;
}

uint32 ModelAnimator::GetCurrentAnimationIndex() const
{
    return m_tweenDesc.m_curr.m_animIndex;
}

wstring ModelAnimator::GetCurrentAnimationTag() const
{
    if (m_tweenDesc.m_curr.m_animIndex < m_indexToTag.size())
        return m_indexToTag[m_tweenDesc.m_curr.m_animIndex];
    return L"";
}

bool ModelAnimator::IsAnimationTransitioning() const
{
    return m_tweenDesc.m_next.m_animIndex >= 0;
}

bool ModelAnimator::IsAnimationFinished() const
{
    if (m_tweenDesc.m_next.m_animIndex >= 0)
        return false;

    wstring currentTag = m_indexToTag[m_tweenDesc.m_curr.m_animIndex];
    shared_ptr<ModelAnimation> currentAnim = m_model->GetAnimationByTag(currentTag);
    if (currentAnim)
    {
        return m_tweenDesc.m_curr.m_currFrame >= currentAnim->m_frameCount - 1;
    }
    return true;
}

uint32 ModelAnimator::GetAnimationIndexByTag(const wstring& _tag)
{
    auto it = m_tagToIndex.find(_tag);
    if (it != m_tagToIndex.end())
        return it->second;
    return 0;
}

void ModelAnimator::UpdateAnimationState()
{
    // 스킬 실행 중이면 다른 입력 무시
    if (m_isSkillActive)
        return;

    // 방향키 입력 확인
    bool isMoving = INPUT->GetButton(KEY_TYPE::UP) ||
        INPUT->GetButton(KEY_TYPE::DOWN) ||
        INPUT->GetButton(KEY_TYPE::LEFT) ||
        INPUT->GetButton(KEY_TYPE::RIGHT);

    // Q키 입력 확인 (Wait나 Run 상태에서만)
    if (INPUT->GetButtonDown(KEY_TYPE::Q) &&
        (m_currentState == AnimationState::Wait || m_currentState == AnimationState::Run))
    {
        shared_ptr<ModelAnimation> temp = m_model->GetAnimationByTag(m_stateToTag[AnimationState::Skill]);

        uint32 frameCount = temp->m_frameCount;
        float frameRate = temp->m_frameRate;


        m_isSkillActive = true;




        m_skillCooldown = frameCount / frameRate; // 스킬 지속시간동안 쿨타임
        TransitionToAnimation(AnimationState::Skill);
        return;
    }

    // 이동 상태 변화 처리
    if (isMoving && !m_wasMoving)
    {
        // 방향키를 눌렀을 때 -> Run으로 전환
        TransitionToAnimation(AnimationState::Run);
    }
    else if (!isMoving && m_wasMoving)
    {
        // 방향키를 뗐을 때 -> Wait로 전환
        TransitionToAnimation(AnimationState::Wait);
    }

    // 이전 상태 저장
    m_wasMoving = isMoving;
}

void ModelAnimator::TransitionToAnimation(AnimationState newState)
{
    if (m_currentState == newState || !CanTransitionTo(newState))
        return;

    m_currentState = newState;
    wstring animTag = m_stateToTag[newState];

    // 태그 기반으로 애니메이션 변경
    SetAnimationByTag(animTag, false);

    // 상태별 속도 설정
    if (m_animationSpeeds.find(newState) != m_animationSpeeds.end())
    {
        SetAnimationSpeed(m_animationSpeeds[newState]);
    }
}

bool ModelAnimator::CanTransitionTo(AnimationState newState) const
{
    // 스킬 실행 중에는 다른 상태로 전환 불가
    if (m_isSkillActive && newState != AnimationState::Skill)
        return false;

    return true;
}
