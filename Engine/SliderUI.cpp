#include "pch.h"
#include "SliderUI.h"
#include "MeshRenderer.h"

SliderUI::SliderUI() : Super(ComponentType::SLIDER)
{
}

SliderUI::~SliderUI()
{
    m_isDestroying = true;

    // Scene의 지연 삭제 시스템 사용
    if (CURSCENE && !CURSCENE->IsDestroying()) {
        if (m_trackObject) {
            CURSCENE->GetObjectManager()->MarkUIObjectForDestroy(m_trackObject);
        }
        if (m_handleObject) {
            CURSCENE->GetObjectManager()->MarkUIObjectForDestroy(m_handleObject);
        }
    }
}

void SliderUI::Init()
{
    auto go = GetGameObject();

    if (go->GetMeshRenderer() == nullptr) {
        go->AddComponent(make_shared<MeshRenderer>());
    }

    go->SetLayerIndex(LAYER_UI);
}

void SliderUI::Update()
{
    Super::Update();

    if (m_isDestroying || !m_isEnabled || !m_visible)
        return;

    HandleInput();
}

void SliderUI::Create(Vec2 localPos, Vec2 size, shared_ptr<Material> trackMaterial, shared_ptr<Material> handleMaterial, float minValue, float maxValue)
{
    m_localPosition = localPos;
    m_trackSize = size;
    m_minValue = minValue;
    m_maxValue = maxValue;
    m_currentValue = minValue;

    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();

    // Track GameObject 생성
    m_trackObject = make_shared<GameObject>();
    m_trackObject->SetName(L"SliderTrack");
    m_trackObject->SetLayerIndex(LAYER_UI);
    m_trackObject->AddComponent(make_shared<MeshRenderer>());

    auto mesh = RESOURCES->Get<Mesh>(L"Quad");
    m_trackObject->GetMeshRenderer()->SetMesh(mesh);
    m_trackObject->GetMeshRenderer()->SetMaterial(trackMaterial);
    m_trackObject->GetMeshRenderer()->SetPass(0);

    float trackX = m_localPosition.x - width / 2.0f;
    float trackY = height / 2.0f - m_localPosition.y;
    m_trackObject->GetTransform()->SetPosition(Vec3(trackX, trackY, m_zTrackPos));
    m_trackObject->GetTransform()->SetScale(Vec3(m_trackSize.x, m_trackSize.y, 1));

    // Handle GameObject 생성
    m_handleObject = make_shared<GameObject>();
    m_handleObject->SetName(L"SliderHandle");
    m_handleObject->SetLayerIndex(LAYER_UI);
    m_handleObject->AddComponent(make_shared<MeshRenderer>());

    m_handleObject->GetMeshRenderer()->SetMesh(mesh);
    m_handleObject->GetMeshRenderer()->SetMaterial(handleMaterial);
    m_handleObject->GetMeshRenderer()->SetPass(0);
    m_handleObject->GetTransform()->SetScale(Vec3(m_handleSize.x, m_handleSize.y, 1));

    // Scene에 UI 객체로 등록
    if (CURSCENE) {
        CURSCENE->AddUIObject(m_trackObject, false);
        CURSCENE->RegisterUIChild(m_trackObject);

        CURSCENE->AddUIObject(m_handleObject, false);
        CURSCENE->RegisterUIChild(m_handleObject);
    }

    // 피킹용 RECT 초기화
    m_trackRect.left = static_cast<LONG>(m_localPosition.x - m_trackSize.x / 2);
    m_trackRect.right = static_cast<LONG>(m_localPosition.x + m_trackSize.x / 2);
    m_trackRect.top = static_cast<LONG>(m_localPosition.y - m_trackSize.y / 2);
    m_trackRect.bottom = static_cast<LONG>(m_localPosition.y + m_trackSize.y / 2);

    UpdateHandlePosition();

    m_isEnabled = true;
    m_visible = true;
    m_isDragging = false;
}

void SliderUI::SetValue(float value)
{
    float clampedValue = std::clamp(value, m_minValue, m_maxValue);

    if (fabs(clampedValue - m_currentValue) > 0.0001f) {
        m_currentValue = clampedValue;
        UpdateHandlePosition();
        OnValueChanged(m_currentValue);
    }
}

void SliderUI::SetRange(float minValue, float maxValue)
{
    m_minValue = minValue;
    m_maxValue = maxValue;

    // 현재 값이 범위를 벗어나면 클램프
    if (m_currentValue < m_minValue || m_currentValue > m_maxValue) {
        SetValue(m_currentValue);
    }
}

void SliderUI::UpdatePosition(const Vec2& parentWorldPos)
{
    Vec2 newWorldPos;
    newWorldPos.x = parentWorldPos.x + m_localPosition.x;
    newWorldPos.y = parentWorldPos.y + m_localPosition.y;

    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();

    // 트랙 위치 업데이트
    if (m_trackObject) {
        float x = newWorldPos.x - width / 2.0f;
        float y = height / 2.0f - newWorldPos.y;
        m_trackObject->GetTransform()->SetPosition(Vec3(x, y, m_zTrackPos));
    }

    // 로컬 위치 업데이트 후 핸들 위치 재계산
    m_localPosition = newWorldPos;
    UpdateHandlePosition();

    // 트랙 피킹 RECT 업데이트
    m_trackRect.left = static_cast<LONG>(m_localPosition.x - m_trackSize.x / 2);
    m_trackRect.right = static_cast<LONG>(m_localPosition.x + m_trackSize.x / 2);
    m_trackRect.top = static_cast<LONG>(m_localPosition.y - m_trackSize.y / 2);
    m_trackRect.bottom = static_cast<LONG>(m_localPosition.y + m_trackSize.y / 2);
}

void SliderUI::HandleInput()
{
    if (!m_handleObject || !m_trackObject) return;

    POINT mousePos = INPUT->GetMousePos();
    bool leftDown = INPUT->GetButtonDown(KEY_TYPE::LBUTTON);
    bool leftPressed = INPUT->GetButton(KEY_TYPE::LBUTTON);
    bool leftUp = INPUT->GetButtonUp(KEY_TYPE::LBUTTON);

    if (!m_isDragging) {
        if (leftDown && IsPointInHandle(mousePos)) {
            m_isDragging = true;

            // 드래그 오프셋 계산
            float mouseScreenPos = (m_direction == SliderDirection::HORIZONTAL) ?
                (float)mousePos.x : (float)mousePos.y;
            float handleScreenPos = ValueToScreen(m_currentValue);

            if (m_direction == SliderDirection::HORIZONTAL) {
                m_dragOffset.x = mouseScreenPos - handleScreenPos;
                m_dragOffset.y = 0;
            }
            else {
                m_dragOffset.x = 0;
                m_dragOffset.y = mouseScreenPos - handleScreenPos;
            }

            OnDragStart(m_currentValue);
        }
        else if (leftDown && IsPointInTrack(mousePos)) {
            // 트랙 클릭 시 핸들을 해당 위치로 점프
            float pos = (m_direction == SliderDirection::HORIZONTAL) ?
                (float)mousePos.x : (float)mousePos.y;
            float newValue = ScreenToValue(pos);
            SetValue(newValue);
        }
    }
    else {
        if (leftPressed) {
            // 드래그 중
            float pos = (m_direction == SliderDirection::HORIZONTAL) ?
                (float)mousePos.x : (float)mousePos.y;
            pos -= (m_direction == SliderDirection::HORIZONTAL) ?
                m_dragOffset.x : m_dragOffset.y;

            float newValue = ScreenToValue(pos);
            SetValue(newValue);
        }

        if (leftUp) {
            // 드래그 종료
            m_isDragging = false;
            OnDragEnd(m_currentValue);
        }
    }
}

void SliderUI::UpdateHandlePosition()
{
    if (!m_handleObject) return;

    float screenPos = ValueToScreen(m_currentValue);
    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();

    float x, y;

    if (m_direction == SliderDirection::HORIZONTAL) {
        x = screenPos - width / 2.0f;
        y = height / 2.0f - m_localPosition.y;
    }
    else {
        x = m_localPosition.x - width / 2.0f;
        y = height / 2.0f - screenPos;
    }

    m_handleObject->GetTransform()->SetPosition(Vec3(x, y, m_zHandlePos));

    // 피킹용 RECT 업데이트
    if (m_direction == SliderDirection::HORIZONTAL) {
        m_handleRect.left = static_cast<LONG>(screenPos - m_handleSize.x / 2);
        m_handleRect.right = static_cast<LONG>(screenPos + m_handleSize.x / 2);
        m_handleRect.top = static_cast<LONG>(m_localPosition.y - m_handleSize.y / 2);
        m_handleRect.bottom = static_cast<LONG>(m_localPosition.y + m_handleSize.y / 2);
    }
    else {
        m_handleRect.left = static_cast<LONG>(m_localPosition.x - m_handleSize.x / 2);
        m_handleRect.right = static_cast<LONG>(m_localPosition.x + m_handleSize.x / 2);
        m_handleRect.top = static_cast<LONG>(screenPos - m_handleSize.y / 2);
        m_handleRect.bottom = static_cast<LONG>(screenPos + m_handleSize.y / 2);
    }
}

float SliderUI::ScreenToValue(float screenPos)
{
    float trackStart, trackEnd;

    if (m_direction == SliderDirection::HORIZONTAL) {
        trackStart = m_localPosition.x - m_trackSize.x / 2.0f;
        trackEnd = m_localPosition.x + m_trackSize.x / 2.0f;
    }
    else {
        trackStart = m_localPosition.y - m_trackSize.y / 2.0f;
        trackEnd = m_localPosition.y + m_trackSize.y / 2.0f;
    }

    // 클램프
    screenPos = std::clamp(screenPos, trackStart, trackEnd);

    float t = (screenPos - trackStart) / (trackEnd - trackStart);

    // 세로 슬라이더의 경우 Y축 반전
    if (m_direction == SliderDirection::VERTICAL) {
        t = 1.0f - t;
    }

    return m_minValue + t * (m_maxValue - m_minValue);
}

float SliderUI::ValueToScreen(float value)
{
    float trackStart, trackEnd;

    if (m_direction == SliderDirection::HORIZONTAL) {
        trackStart = m_localPosition.x - m_trackSize.x / 2.0f;
        trackEnd = m_localPosition.x + m_trackSize.x / 2.0f;
    }
    else {
        trackStart = m_localPosition.y - m_trackSize.y / 2.0f;
        trackEnd = m_localPosition.y + m_trackSize.y / 2.0f;
    }

    float t = (value - m_minValue) / (m_maxValue - m_minValue);

    // 세로 슬라이더의 경우 Y축 반전
    if (m_direction == SliderDirection::VERTICAL) {
        t = 1.0f - t;
    }

    return trackStart + t * (trackEnd - trackStart);
}

bool SliderUI::IsPointInTrack(POINT point)
{
    return ::PtInRect(&m_trackRect, point);
}

bool SliderUI::IsPointInHandle(POINT point)
{
    return ::PtInRect(&m_handleRect, point);
}
