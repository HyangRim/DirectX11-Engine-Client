#pragma once
#include "Renderer.h"

// ModelAnimator.h에 추가
enum class AnimationState
{
    Wait = 0,
    Run = 1,
    BaseAttack = 2,
    Skill = 3
};

// 애니메이션 시퀀스 구조체
struct AnimationSequence
{
    wstring sequenceName;                           // 시퀀스 이름
    vector<wstring> animationTags;                  // 재생할 애니메이션 태그들
    vector<float> transitionDurations;             // 각 전환 시간
    bool isLoop = false;                            // 시퀀스 반복 여부
    uint32 currentIndex = 0;                       // 현재 재생 중인 애니메이션 인덱스
    bool isPlaying = false;                         // 시퀀스 재생 중 여부
    function<void()> onSequenceComplete = nullptr; // 시퀀스 완료 콜백
};

class Model;

struct AnimTransform {
    // 200개의 관절 정보를 담아줄 수 있음.
    using TransformArrayType = array<Matrix, MAX_BONE_TRANSFORMS>;

    //250개의 본 * 500개의 키프레임. 
    //2차 배열. 
    array<TransformArrayType, MAX_MODEL_KEYFRAMES> transforms;
};

class ModelAnimator :
    public Renderer
{
    using Super = Renderer;

public:
    ModelAnimator(shared_ptr<Shader> _shader);
    ~ModelAnimator();

    virtual void Update() override;
    void UpdateTweenData();
    void SetModel(shared_ptr<Model> _model);
    void SetPass(uint8 _pass) { m_pass = _pass; }
    shared_ptr<Shader> GetShader();

    //애니메이션 제어 메소드 - 태그 기반 추가
    void SetAnimation(uint32 _animIndex, bool _immediate = false);
    void SetAnimationByTag(const wstring& _tag, bool _immediate = false);  // 새로운 메서드

    //duration은 _tweenDuration초 동안 부드럽게 전환. 
    void SetNextAnimation(uint32 _animIndex, bool _tweenDuration = 1.0f);
    void SetNextAnimationByTag(const wstring& _tag, bool _tweenDuration = 1.0f);  // 새로운 메서드
    void SetAnimationSpeed(float _speed);

    //애니메이션 상태 Get
    uint32 GetCurrentAnimationIndex() const;
    wstring GetCurrentAnimationTag() const;  // 새로운 메서드
    bool IsAnimationTransitioning() const;
    bool IsAnimationFinished() const;

    void RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer, bool _isShadowTech);
    InstanceID GetInstanceID();
    TweenDesc GetTweenDesc() { return m_tweenDesc; }

private:
    void CreateTexture();
    void CreateAnimationTransform(const wstring& _tag);  // 태그 기반으로 변경
    //void CreateAnimationTransform(uint32 _index);  // 기존 버전 주석처리
    //void CreateAnimationTransform(shared_ptr<ModelAnimation> _modelAnim);

    uint32 GetAnimationIndexByTag(const wstring& _tag);  // 내부 변환 메서드

private:
    //vector<AnimTransform> m_animTransform;  // 기존 버전 주석처리
    unordered_map<wstring, AnimTransform> m_animTransform;  // 새로운 버전
    ComPtr<ID3D11Texture2D> m_texture;
    ComPtr<ID3D11ShaderResourceView> m_srv;

    // 태그와 인덱스 매핑
    unordered_map<wstring, uint32> m_tagToIndex;
    vector<wstring> m_indexToTag;

private:
    TweenDesc m_tweenDesc;

private:
    shared_ptr<Shader>  m_shader;
    uint8               m_pass = 0;
    shared_ptr<Model>   m_model;

    //////////////변환////////////////

public:
    // 기존 상태 관리 변수들...
    AnimationState m_currentState = AnimationState::Wait;
    AnimationState m_nextState = AnimationState::Wait;
    bool m_isTransitioning = false;
    float m_transitionDuration = 0.25f;
    float m_transitionTimer = 0.0f;

    // 키 입력 상태 관리 변수들
    bool m_wasMoving = false;  // 이전 프레임에서 움직였는지 확인
    bool m_isSkillActive = false;  // 스킬 실행 중인지 확인
    float m_skillCooldown = 0.0f;  // 스킬 쿨다운 타이머

    // 상태별 설정 - 태그 기반으로 변경
    map<AnimationState, wstring> m_stateToTag;  // 상태를 태그로 매핑
    map<AnimationState, float> m_animationSpeeds;
    map<AnimationState, bool> m_loopSettings;

    // 애니메이션 상태 관리 메서드들
    void UpdateAnimationState();
    void TransitionToAnimation(AnimationState newState);
    bool CanTransitionTo(AnimationState newState) const;





    //시퀀스

};
