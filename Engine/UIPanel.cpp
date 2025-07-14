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
    try {
        // OnDestroy가 아직 호출되지 않았다면 호출
        OnDestroy();

#ifdef _DEBUG
        std::cout << "UIPanel 소멸자 완료" << std::endl;
#endif

    }
    catch (...) {
#ifdef _DEBUG
        std::cout << "UIPanel 소멸자에서 예외 발생" << std::endl;
#endif
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
        for (auto it = m_childElements.begin(); it != m_childElements.end();) {
            if (auto child = it->lock()) {
                CURSCENE->Remove(child);
                child->OnDestroy(); // 명시적 소멸 호출
                ++it;
            }
            else {
                it = m_childElements.erase(it);
            }
        }

        // 2. 컨테이너 완전 정리
        m_childElements.clear();
        m_namedElements.clear();

        // 3. 배경 리소스 정리
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
    catch (...) {
#ifdef _DEBUG
        std::cout << "UIPanel::OnDestroy에서 예외 발생" << std::endl;
#endif
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

    // 자식 요소로 등록 (weak_ptr 사용)
    m_childElements.push_back(textObj);
    m_namedElements[name] = textObj;

    // 씬에 추가
    CURSCENE->Add(textObj);

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

    // 월드 좌표로 변환하여 위치 설정
    Vec2 worldPos = LocalToWorldPosition(localPos);

    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();

    float x = worldPos.x - width / 2.0f;
    float y = height / 2.0f - worldPos.y;

    imageUIObj->GetTransform()->SetPosition(Vec3(x, y, -0.05f)); // 패널과 버튼 사이

    imageUIObj->SetLayerIndex(LAYER_UI);

    // 자식 요소로 등록 (weak_ptr 사용)
    m_childElements.push_back(imageUIObj);
    m_namedElements[name] = imageUIObj;

    // 씬에 추가
    CURSCENE->Add(imageUIObj);

    return imageUIComponent;
}

void UIPanel::RemoveUIElement(const wstring& name)
{
    auto it = m_namedElements.find(name);
    if (it != m_namedElements.end()) {
        // weak_ptr을 shared_ptr로 변환
        if (auto child = it->second.lock()) {
            // 벡터에서 제거 (weak_ptr 비교)
            auto vecIt = std::find_if(m_childElements.begin(), m_childElements.end(),
                [&child](const weak_ptr<GameObject>& weakPtr) {
                    return !weakPtr.owner_before(child) && !child.owner_before(weakPtr);
                });

            if (vecIt != m_childElements.end()) {
                m_childElements.erase(vecIt);
            }

            // 씬에서 제거
            CURSCENE->Remove(child);
        }

        // 맵에서 제거
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
