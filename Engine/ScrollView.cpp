#include "pch.h"
#include "ScrollView.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "Shader.h"
#include "GameObject.h"
#include "UIPanel.h"
#include "Scene.h"
#include "SceneObjectManager.h"
#include "Graphics.h"

ScrollView::ScrollView() : Super(ComponentType::ScrollView)
{
}

ScrollView::~ScrollView()
{
    m_isDestroying = true;
}

void ScrollView::Init()
{
    auto go = GetGameObject();

    // MeshRenderer 추가 (배경용)
    if (go->GetMeshRenderer() == nullptr) {
        go->AddComponent(make_shared<MeshRenderer>());
    }

    // 배경 메시 생성
    m_backgroundMesh = RESOURCES->Get<Mesh>(L"Quad");
    go->GetMeshRenderer()->SetMesh(m_backgroundMesh);
    go->SetLayerIndex(LAYER_UI);

    CreateScrollView();
}

void ScrollView::Update()
{
    Super::Update();

    if (m_isDestroying) return;

    // InputManager를 사용한 입력 처리
    HandleInput();

    // 스크롤 위치 업데이트
    UpdateScrollPosition();
    UpdateContentPosition();

    // 만료된 컨텐츠 요소 정리
    for (auto it = m_contentElements.begin(); it != m_contentElements.end();) {
        if (it->expired()) {
            it = m_contentElements.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ScrollView::Create(Vec2 screenPos, Vec2 viewSize, shared_ptr<Material> backgroundMaterial)
{
    Init();

    m_position = screenPos;
    m_viewSize = viewSize;

    auto go = GetGameObject();

    // 위치 설정
    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();

    float x = screenPos.x - width / 2.0f;
    float y = height / 2.0f - screenPos.y;

    go->GetTransform()->SetPosition(Vec3(x, y, 0.0f));
    go->GetTransform()->SetScale(Vec3(viewSize.x, viewSize.y, 1.0f));

    // 뷰포트 영역 설정
    m_viewportRect.left = static_cast<LONG>(screenPos.x - viewSize.x / 2.0f);
    m_viewportRect.right = static_cast<LONG>(screenPos.x + viewSize.x / 2.0f);
    m_viewportRect.top = static_cast<LONG>(screenPos.y - viewSize.y / 2.0f);
    m_viewportRect.bottom = static_cast<LONG>(screenPos.y + viewSize.y / 2.0f);

    // 배경 머티리얼 설정
    if (backgroundMaterial) {
        m_backgroundMaterial = backgroundMaterial;

        go->GetMeshRenderer()->SetMaterial(m_backgroundMaterial);
        go->GetMeshRenderer()->SetPass(1);
    }
    else {
        // 기본 배경 머티리얼 생성
        m_backgroundMaterial = make_shared<Material>();
        auto shader = make_shared<Shader>(L"ImageShader.fx");
        m_backgroundMaterial->SetShader(shader);
        m_backgroundMaterial->SetRenderQueue(RenderQueue::Transparent);
        m_backgroundMaterial->SetTransparent(true);
        m_backgroundMaterial->SetRenderingMode(RenderingMode::Forward);

        MaterialDesc& desc = m_backgroundMaterial->GetMaterialDesc();
        desc.ambient = Vec4(0.f, 0.f, 0.f, 0.7f);
        desc.diffuse = Vec4(0.f, 0.f, 0.f, 0.7f);
        desc.specular = Vec4(0.f);
        desc.emissive = Vec4(0.f);

        go->GetMeshRenderer()->SetMaterial(m_backgroundMaterial);
        go->GetMeshRenderer()->SetPass(2);
    }

  
}

void ScrollView::SetContentSize(Vec2 contentSize)
{
    m_contentSize = contentSize;

    // 최대 스크롤 위치 계산
    m_maxScrollPosition.x = max(0.0f, m_contentSize.x - m_viewSize.x);
    m_maxScrollPosition.y = max(0.0f, m_contentSize.y - m_viewSize.y);

    ClampScrollPosition();
}

void ScrollView::ScrollTo(Vec2 position)
{
    m_scrollPosition = position;
    ClampScrollPosition();
}

void ScrollView::ScrollBy(Vec2 delta)
{
    m_scrollPosition += delta;
    ClampScrollPosition();
}

void ScrollView::ScrollToTop()
{
    m_scrollPosition.y = 0.0f;
}

void ScrollView::ScrollToBottom()
{
    m_scrollPosition.y = m_maxScrollPosition.y;
}

void ScrollView::ScrollToLeft()
{
    m_scrollPosition.x = 0.0f;
}

void ScrollView::ScrollToRight()
{
    m_scrollPosition.x = m_maxScrollPosition.x;
}

shared_ptr<UIPanel> ScrollView::AddPanel(Vec2 localPos, Vec2 size, shared_ptr<Material> material, const wstring& name)
{
    if (m_isDestroying) return nullptr;

    // 패널 GameObject 생성
    auto panelObj = make_shared<GameObject>();
    panelObj->GetTransform()->SetPosition(Vec3(localPos.x, localPos.y, 0));
    panelObj->SetName(name);

    // UIPanel 컴포넌트 추가
    auto panelComponent = make_shared<UIPanel>();
    panelObj->AddComponent(panelComponent);

    // 원래 위치 저장
    m_originalPositions[panelObj] = localPos;

    // 컨텐츠 좌표를 화면 좌표로 변환
    Vec2 screenPos = ContentToScreenPosition(localPos);
    panelComponent->Create(screenPos, size, material);

    panelObj->SetLayerIndex(LAYER_UI);

    // 컨텐츠 요소로 등록
    m_contentElements.push_back(panelObj);
    m_namedElements[name] = panelObj;

    // UI 객체로 씬에 추가
    CURSCENE->AddUIObject(panelObj, false);
    CURSCENE->RegisterUIChild(panelObj);

    return panelComponent;
}

void ScrollView::AddUIElement(shared_ptr<GameObject> uiElement, Vec2 localPos)
{
    if (m_isDestroying || !uiElement) return;

    // 원래 위치 저장 (중요!)
    m_originalPositions[uiElement] = localPos;

    // 컨텐츠 좌표를 화면 좌표로 변환하여 위치 설정
    Vec2 screenPos = ContentToScreenPosition(localPos);

    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();

    float x = screenPos.x - width / 2.0f;
    float y = height / 2.0f - screenPos.y;

    uiElement->GetTransform()->SetPosition(Vec3(x, y, -0.1f));
    uiElement->SetLayerIndex(LAYER_UI);

    // 컨텐츠 요소로 등록
    m_contentElements.push_back(uiElement);

    // UI 객체로 씬에 추가
    CURSCENE->AddUIObject(uiElement, false);
    CURSCENE->RegisterUIChild(uiElement);
}

void ScrollView::RemoveUIElement(shared_ptr<GameObject> uiElement)
{
    if (m_isDestroying || !uiElement) return;

    // 컨텐츠 요소에서 제거
    auto it = std::find_if(m_contentElements.begin(), m_contentElements.end(),
        [&uiElement](const weak_ptr<GameObject>& weakPtr) {
            return !weakPtr.owner_before(uiElement) && !uiElement.owner_before(weakPtr);
        });

    if (it != m_contentElements.end()) {
        m_contentElements.erase(it);
    }

    // 지연 삭제 사용
    if (CURSCENE && !CURSCENE->IsDestroying()) {
        CURSCENE->GetObjectManager()->MarkUIObjectForDestroy(uiElement);
    }
}

void ScrollView::HandleInput()
{
    // InputManager를 사용한 마우스 입력 처리
    POINT mousePos = INPUT->GetMousePos();
    bool isPressed = INPUT->GetButton(KEY_TYPE::LBUTTON);
    bool isDown = INPUT->GetButtonDown(KEY_TYPE::LBUTTON);
    bool isUp = INPUT->GetButtonUp(KEY_TYPE::LBUTTON);

    // 마우스 휠 처리
    if (INPUT->HasWheelInput()) {
        if (IsPointInViewport(mousePos)) {
            int wheelDelta = INPUT->GetMouseWheelDelta();
            bool handled = HandleMouseWheel(wheelDelta);
        }

        // 휠 이벤트 소비
        INPUT->ResetWheelDelta();
    }

    // 기존 드래그 처리
    if (IsPointInViewport(mousePos)) {
        if (isDown && !m_isDragging) {
            m_isDragging = true;
            m_lastMousePosition = Vec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        }
    }

    if (m_isDragging) {
        if (isUp) {
            m_isDragging = false;
        }
        else if (isPressed) {
            Vec2 currentMousePos = Vec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            Vec2 delta = currentMousePos - m_lastMousePosition;

            Vec2 scrollDelta = Vec2::Zero;
            if (m_scrollDirection == ScrollDirection::Vertical || m_scrollDirection == ScrollDirection::Both) {
                scrollDelta.y = -delta.y;
            }
            if (m_scrollDirection == ScrollDirection::Horizontal || m_scrollDirection == ScrollDirection::Both) {
                scrollDelta.x = -delta.x;
            }

            if (scrollDelta.x != 0.0f || scrollDelta.y != 0.0f) {
                ScrollBy(scrollDelta);
            }
            m_lastMousePosition = currentMousePos;
        }
    }
}

void ScrollView::CreateScrollView()
{
    // 컨텐츠 컨테이너는 실제로는 가상적인 개념
    // 실제 UI 요소들의 위치를 스크롤 위치에 따라 조정
}

void ScrollView::UpdateScrollPosition()
{
    ClampScrollPosition();
}

void ScrollView::UpdateContentPosition()
{
    if (m_isDestroying) return;

    for (auto it = m_contentElements.begin(); it != m_contentElements.end();) {
        if (auto element = it->lock()) {
            // 원래 위치에서 스크롤 오프셋 적용
            auto posIt = m_originalPositions.find(element);
            if (posIt != m_originalPositions.end()) {
                Vec2 originalContentPos = posIt->second;
                Vec2 screenPos = ContentToScreenPosition(originalContentPos);

                //cout << "아마 panel : " << screenPos.x << " , " << screenPos.y << endl;

                float height = GRAPHICS->GetViewport().GetHeight();
                float width = GRAPHICS->GetViewport().GetWidth();

                float x = screenPos.x - width / 2.0f;
                float y = height / 2.0f - screenPos.y;

                element->GetTransform()->SetPosition(Vec3(x, y, -0.1f));
            }
            ++it;
        }
        else {
            it = m_contentElements.erase(it);
        }
    }
}

void ScrollView::ClampScrollPosition()
{
    m_scrollPosition.x = Clamp(m_scrollPosition.x, 0.0f, m_maxScrollPosition.x);
    m_scrollPosition.y = Clamp(m_scrollPosition.y, 0.0f, m_maxScrollPosition.y);
}

bool ScrollView::IsPointInViewport(POINT point)
{
    return PtInRect(&m_viewportRect, point);
}

Vec2 ScrollView::ScreenToContentPosition(Vec2 screenPos)
{
    // 화면 좌표를 컨텐츠 로컬 좌표로 변환
    Vec2 viewportLocalPos = screenPos - m_position;
    Vec2 contentPos = viewportLocalPos + m_scrollPosition;
    return contentPos;
}

Vec2 ScrollView::ContentToScreenPosition(Vec2 contentPos)
{
    // 컨텐츠 로컬 좌표를 화면 좌표로 변환
    Vec2 viewportLocalPos = contentPos - m_scrollPosition;
    Vec2 screenPos = viewportLocalPos + m_position;
    return screenPos;
}

void ScrollView::OnDestroy()
{
    m_isDestroying = true;

    // 컨텐츠 요소들 정리
    for (auto& weakElement : m_contentElements) {
        if (auto element = weakElement.lock()) {
            if (CURSCENE && !CURSCENE->IsDestroying()) {
                CURSCENE->GetObjectManager()->MarkUIObjectForDestroy(element);
            }
        }
    }

    // 컨테이너 정리
    m_contentElements.clear();
    m_namedElements.clear();

    Super::OnDestroy();
}

bool ScrollView::HandleMouseWheel(int wheelDelta)
{
    POINT mousePos = INPUT->GetMousePos();
    if (!IsPointInViewport(mousePos)) return false;

    float scrollAmount = (wheelDelta > 0) ? -m_scrollSpeed : m_scrollSpeed;
    Vec2 scrollDelta = Vec2::Zero;

    if (m_scrollDirection == ScrollDirection::Vertical || m_scrollDirection == ScrollDirection::Both) {
        scrollDelta.y = scrollAmount;
    }

    ScrollBy(scrollDelta);
    return true;
}
