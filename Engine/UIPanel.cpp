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

UIPanel::UIPanel() : Super(ComponentType::UIPanel)
{
}

UIPanel::~UIPanel()
{
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

    // 자식 요소들의 가시성 업데이트
    for (auto& child : m_childElements) {
        if (child) {
            // 패널이 보이지 않으면 자식들도 숨김
            // 실제로는 Transform의 활성화/비활성화로 처리
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
    }
    else {
        // 기본 배경 머티리얼 생성
        m_backgroundMaterial = make_shared<Material>();
        auto shader = make_shared<Shader>(L"23. RenderDemo.fx");
        m_backgroundMaterial->SetShader(shader);
        m_backgroundMaterial->SetRenderQueue(RenderQueue::Transparent);

        // 기본 색상 설정
        MaterialDesc& desc = m_backgroundMaterial->GetMaterialDesc();
        desc.ambient = m_backgroundColor;
        desc.diffuse = m_backgroundColor;
        desc.specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        desc.emissive = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    go->GetMeshRenderer()->SetMaterial(m_backgroundMaterial);
    go->SetLayerIndex(LAYER_UI);
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

    // 자식 요소들의 가시성도 함께 설정
    for (auto& child : m_childElements) {
        if (child) {
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

    // 자식 요소로 등록
    m_childElements.push_back(buttonObj);
    m_namedElements[name] = buttonObj;

    // 씬에 추가
    CURSCENE->Add(buttonObj);

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
        -0.2f  // 버튼보다도 앞쪽
    ));

    textObj->SetLayerIndex(LAYER_UI);

    // 자식 요소로 등록
    m_childElements.push_back(textObj);
    m_namedElements[name] = textObj;

    // 씬에 추가
    CURSCENE->Add(textObj);

    return textComponent;
}

void UIPanel::RemoveUIElement(const wstring& name)
{
    auto it = m_namedElements.find(name);
    if (it != m_namedElements.end()) {
        // 벡터에서 제거
        auto vecIt = std::find(m_childElements.begin(), m_childElements.end(), it->second);
        if (vecIt != m_childElements.end()) {
            m_childElements.erase(vecIt);
        }

        // 씬에서 제거
        CURSCENE->Remove(it->second);

        // 맵에서 제거
        m_namedElements.erase(it);
    }
}

shared_ptr<Button> UIPanel::GetButton(const wstring& name)
{
    auto it = m_namedElements.find(name);
    if (it != m_namedElements.end()) {
        return it->second->GetButton();
    }
    return nullptr;
}

shared_ptr<Text> UIPanel::GetText(const wstring& name)
{
    auto it = m_namedElements.find(name);
    if (it != m_namedElements.end()) {
        return it->second->GetText();
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
    // 실제로는 각 자식의 로컬 위치를 기억해두고 다시 계산해야 함
}

Vec2 UIPanel::LocalToWorldPosition(const Vec2& localPos)
{
    // 패널 내의 로컬 좌표를 월드 화면 좌표로 변환
    Vec2 worldPos;
    worldPos.x = m_position.x + localPos.x - (m_size.x / 2.0f);
    worldPos.y = m_position.y + localPos.y - (m_size.y / 2.0f);
    return worldPos;
}
