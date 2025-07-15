#pragma once
#include "Component.h"

class Material;
class Mesh;
class Texture;
class Button;
class Text;
class ImageUI;

class UIPanel : public Component
{
    using Super = Component;

public:
    UIPanel();
    virtual ~UIPanel();

    virtual void Init() override;
    virtual void Update() override;

    // 패널 설정 함수들
    void SetPosition(const Vec2& position);
    void SetSize(const Vec2& size);
    void SetBackgroundColor(const Vec4& color);
    void SetBackgroundTexture(shared_ptr<Texture> texture);
    void SetVisible(bool visible);

    // UI 요소 추가 함수들
    shared_ptr<Button> AddButton(Vec2 localPos, Vec2 size, shared_ptr<Material> material, const wstring& name = L"Button");
    shared_ptr<Text> AddText(Vec2 localPos, const wstring& text, float fontSize = 16.0f,
        Vec4 color = Vec4(1, 1, 1, 1), float alpha = 1.0f,
        Vec4 outlineColor = Vec4(0, 0, 0, 1), float outlineWidth = 1.0f,
        const wstring& name = L"Text");
    // ImageUI 추가 함수
    shared_ptr<ImageUI> AddImageUI(Vec2 localPos, const wstring& name = L"ImageUI");

    // UI 요소 관리
    void RemoveUIElement(const wstring& name);
    shared_ptr<Button> GetButton(const wstring& name);
    shared_ptr<Text> GetText(const wstring& name);
    shared_ptr<ImageUI> GetImageUI(const wstring& name);


    // Getter 함수들
    const Vec2& GetPosition() const { return m_position; }
    const Vec2& GetSize() const { return m_size; }
    bool IsVisible() const { return m_visible; }

    // 패널 생성 함수
    void Create(Vec2 screenPos, Vec2 size, shared_ptr<Material> backgroundMaterial = nullptr);

public:
    // 소멸 관련 메서드
    virtual void OnDestroy() override;
    void ClearChildReferences() {
        // weak_ptr은 순환 참조를 만들지 않음
        m_childElements.clear();
        m_namedElements.clear();
    }

private:
    void CreatePanelBackground();
    void UpdateChildPositions();
    Vec2 LocalToWorldPosition(const Vec2& localPos);

private:
    Vec2 m_position = Vec2(0.0f, 0.0f);
    Vec2 m_size = Vec2(200.0f, 150.0f);
    Vec4 m_backgroundColor = Vec4(0.f);
    bool m_visible = true;

    // 배경 렌더링용
    shared_ptr<Texture> m_backgroundTexture;
    shared_ptr<Material> m_backgroundMaterial;
    shared_ptr<Mesh> m_backgroundMesh;

    // 자식 UI 요소들
    vector<weak_ptr<GameObject>> m_childElements;
    map<wstring, weak_ptr<GameObject>> m_namedElements;
};
#include "pch.h"
#include "UIPanel.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "Shader.h"
#include "Button.h"
#include "Text.h"
#include "GameObject.h"
#include "ImageUI.h"
#include "SceneManager.h"
#include "Scene.h"

UIPanel::UIPanel() : Super(ComponentType::UIPanel)
{
}

UIPanel::~UIPanel()
{
    try
    {
        // OnDestroy가 아직 호출되지 않았다면 호출
        //OnDestroy();
    }
    catch (...)
    {

    }
}

void UIPanel::Init()
{
    auto go = GetGameObject();

    // MeshRenderer 추가 (배경용)
    if (go->GetMeshRenderer() == nullptr) {
        go->AddComponent(make_shared<MeshRenderer>());
    }

    // 배경 메시 생성
    CreatePanelBackground();

    // UI 레이어 설정
    go->SetLayerIndex(LAYER_UI);
}

void UIPanel::Update()
{
    Super::Update();

    // 자식 요소들의 가시성 업데이트 (weak_ptr 사용)
    for (auto it = m_childElements.begin(); it != m_childElements.end();) {
        if (auto child = it->lock()) {
            // 패널이 보이지 않으면 자식들도 숨김
            // 실제로는 Transform의 활성화/비활성화로 처리
            ++it;
        }
        else {
            // 만료된 weak_ptr 제거
            it = m_childElements.erase(it);
        }
    }
}

void UIPanel::Create(Vec2 screenPos, Vec2 size, shared_ptr<Material> backgroundMaterial)
{
    Init();
    m_position = screenPos;
    m_size = size;

    SetPosition(screenPos);

    auto go = GetGameObject();
    go->GetTransform()->SetScale(Vec3(size.x, size.y, 1));

    // 배경 머티리얼 설정
    if (backgroundMaterial) {
        m_backgroundMaterial = backgroundMaterial;
        go->GetMeshRenderer()->SetMaterial(m_backgroundMaterial);
    }
    else {
        // 기본 배경 머티리얼 생성
        m_backgroundMaterial = make_shared<Material>();
        auto shader = make_shared<Shader>(L"ImageShader.fx");
        m_backgroundMaterial->SetShader(shader);
        m_backgroundMaterial->SetRenderQueue(RenderQueue::Transparent);

        // 기본 색상 설정
        MaterialDesc& desc = m_backgroundMaterial->GetMaterialDesc();
        desc.ambient = m_backgroundColor;
        desc.diffuse = m_backgroundColor;
        desc.specular = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
        desc.emissive = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    go->GetMeshRenderer()->SetMaterial(m_backgroundMaterial);
    go->SetLayerIndex(LAYER_UI);
}

void UIPanel::OnDestroy()
{
    try {
        // 자식 요소들 정리만 하면 됨 (Scene에서 알아서 순서대로 소멸)
        for (auto it = m_childElements.begin(); it != m_childElements.end();)
        {
            if (auto child = it->lock())
            {
                ++it;
            }
            else {
                it = m_childElements.erase(it);
            }
        }

        // 컨테이너들 정리
        m_childElements.clear();
        m_namedElements.clear();

        // 리소스 해제
        if (m_backgroundMaterial) {
            m_backgroundMaterial.reset();
        }
        if (m_backgroundTexture) {
            m_backgroundTexture.reset();
        }
        if (m_backgroundMesh) {
            m_backgroundMesh.reset();
        }

        Super::OnDestroy();

    }
    catch (...)
    {

    }
}


void UIPanel::SetPosition(const Vec2& position)
{
    m_position = position;

    auto go = GetGameObject();
    if (go) {
        float height = GRAPHICS->GetViewport().GetHeight();
        float width = GRAPHICS->GetViewport().GetWidth();

        float x = position.x - width / 2.0f;
        float y = height / 2.0f - position.y;

        go->GetTransform()->SetPosition(Vec3(x, y, 0.0f));
    }

    // 자식 요소들의 위치도 업데이트
    UpdateChildPositions();
}

void UIPanel::SetSize(const Vec2& size)
{
    m_size = size;

    auto go = GetGameObject();
    if (go) {
        go->GetTransform()->SetScale(Vec3(size.x, size.y, 1));
    }
}

void UIPanel::SetVisible(bool visible)
{
    m_visible = visible;

    auto go = GetGameObject();
    if (go) {
        // 패널 자체의 가시성 설정
        // 실제 구현에서는 렌더링 활성화/비활성화 처리
    }

    // 자식 요소들의 가시성도 함께 설정 (weak_ptr 사용)
    for (auto& weakChild : m_childElements) {
        if (auto child = weakChild.lock()) {
            // 자식 요소들의 가시성 설정
        }
    }
}

shared_ptr<Button> UIPanel::AddButton(Vec2 localPos, Vec2 size, shared_ptr<Material> material, const wstring& name)
{
    // 버튼 GameObject 생성
    auto buttonObj = make_shared<GameObject>();
    buttonObj->SetName(name);

    // Button 컴포넌트 추가
    auto buttonComponent = make_shared<Button>();
    buttonObj->AddComponent(buttonComponent);

    // 월드 좌표로 변환하여 버튼 생성
    Vec2 worldPos = LocalToWorldPosition(localPos);
    buttonComponent->Create(worldPos, size, material);

    // Z 위치를 패널보다 앞쪽으로 설정
    buttonObj->GetTransform()->SetPosition(Vec3(
        buttonObj->GetTransform()->GetPosition().x,
        buttonObj->GetTransform()->GetPosition().y,
        -0.1f  // 패널보다 앞쪽
    ));

    buttonObj->SetLayerIndex(LAYER_UI);

    // 자식 요소로 등록 (weak_ptr 사용)
    m_childElements.push_back(buttonObj);
    m_namedElements[name] = buttonObj;

    // **UI 객체로 씬에 추가 (자식으로 등록)**
    CURSCENE->AddUIObject(buttonObj, false);  // false = 자식
    CURSCENE->RegisterUIChild(buttonObj);

    return buttonComponent;
}

shared_ptr<Text> UIPanel::AddText(Vec2 localPos, const wstring& text, float fontSize,
    Vec4 color, float alpha, Vec4 outlineColor, float outlineWidth, const wstring& name)
{
    // 텍스트 GameObject 생성
    auto textObj = make_shared<GameObject>();
    textObj->SetName(name);

    // Text 컴포넌트 추가
    auto textComponent = make_shared<Text>();
    textObj->AddComponent(textComponent);

    // 월드 좌표로 변환하여 텍스트 생성
    Vec2 worldPos = LocalToWorldPosition(localPos);
    textComponent->Create(worldPos, text, fontSize, color, alpha, outlineColor, outlineWidth);

    // Z 위치를 패널보다 앞쪽으로 설정
    textObj->GetTransform()->SetPosition(Vec3(
        textObj->GetTransform()->GetPosition().x,
        textObj->GetTransform()->GetPosition().y,
        -0.15f  // 버튼보다도 앞쪽
    ));

    textObj->SetLayerIndex(LAYER_UI);

    // 자식 요소로 등록 (weak_ptr 사용)
    m_childElements.push_back(textObj);
    m_namedElements[name] = textObj;

    // **UI 객체로 씬에 추가 (자식으로 등록)**
    CURSCENE->AddUIObject(textObj, false);  // false = 자식
    CURSCENE->RegisterUIChild(textObj);

    return textComponent;
}

shared_ptr<ImageUI> UIPanel::AddImageUI(Vec2 localPos, const wstring& name)
{
    // ImageUI GameObject 생성
    auto imageUIObj = make_shared<GameObject>();
    imageUIObj->SetName(name);

    // ImageUI 컴포넌트 추가
    auto imageUIComponent = make_shared<ImageUI>();
    imageUIObj->AddComponent(imageUIComponent);

    // 위치 설정
    Vec2 worldPos = LocalToWorldPosition(localPos);
    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();
    float x = worldPos.x - width / 2.0f;
    float y = height / 2.0f - worldPos.y;

    imageUIObj->GetTransform()->SetPosition(Vec3(x, y, -0.05f));
    imageUIObj->SetLayerIndex(LAYER_UI);

    // 자식 요소로 등록 (weak_ptr 사용)
    m_childElements.push_back(imageUIObj);
    m_namedElements[name] = imageUIObj;

    // **UI 객체로 씬에 추가 (자식으로 등록)**
    CURSCENE->AddUIObject(imageUIObj, false);  // false = 자식
    CURSCENE->RegisterUIChild(imageUIObj);

    return imageUIComponent;
}

void UIPanel::RemoveUIElement(const wstring& name)
{
    auto it = m_namedElements.find(name);
    if (it != m_namedElements.end()) {
        if (auto child = it->second.lock()) {
            // 벡터에서 제거
            auto vecIt = std::find_if(m_childElements.begin(), m_childElements.end(),
                [&child](const weak_ptr<GameObject>& weakPtr) {
                    return !weakPtr.owner_before(child) && !child.owner_before(weakPtr);
                });

            if (vecIt != m_childElements.end()) {
                m_childElements.erase(vecIt);
            }

            // 지연 삭제 시스템 사용
            if (CURSCENE && !CURSCENE->m_isDestroying) {
                CURSCENE->MarkUIObjectForDestroy(child);
            }
        }
        m_namedElements.erase(it);
    }
}

shared_ptr<Button> UIPanel::GetButton(const wstring& name)
{
    auto it = m_namedElements.find(name);
    if (it != m_namedElements.end()) {
        if (auto child = it->second.lock()) {
            return child->GetButton();
        }
    }
    return nullptr;
}

shared_ptr<Text> UIPanel::GetText(const wstring& name)
{
    auto it = m_namedElements.find(name);
    if (it != m_namedElements.end()) {
        if (auto child = it->second.lock()) {
            return child->GetText();
        }
    }
    return nullptr;
}

shared_ptr<ImageUI> UIPanel::GetImageUI(const wstring& name)
{
    auto it = m_namedElements.find(name);
    if (it != m_namedElements.end()) {
        if (auto child = it->second.lock()) {
            return child->GetImageUI();
        }
    }
    return nullptr;
}

void UIPanel::CreatePanelBackground()
{
    // Quad 메시 생성
    m_backgroundMesh = make_shared<Mesh>();
    m_backgroundMesh->CreateQuad();

    auto go = GetGameObject();
    go->GetMeshRenderer()->SetMesh(m_backgroundMesh);
    go->GetMeshRenderer()->SetPass(0);
}

void UIPanel::UpdateChildPositions()
{
    // 패널 위치가 변경되면 자식 요소들의 위치도 업데이트
    // weak_ptr을 사용하여 안전하게 접근
    for (auto it = m_childElements.begin(); it != m_childElements.end();) {
        if (auto child = it->lock()) {
            // 자식 위치 업데이트 로직
            ++it;
        }
        else {
            // 만료된 weak_ptr 제거
            it = m_childElements.erase(it);
        }
    }
}

Vec2 UIPanel::LocalToWorldPosition(const Vec2& localPos)
{
    // 패널 내의 로컬 좌표를 월드 화면 좌표로 변환
    Vec2 worldPos;
    worldPos.x = m_position.x + localPos.x - (m_size.x / 2.0f);
    worldPos.y = m_position.y + localPos.y - (m_size.y / 2.0f);
    return worldPos;
}
