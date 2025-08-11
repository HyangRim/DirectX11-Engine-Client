#include "pch.h"
#include "Button.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "ImageUI.h"

Button::Button() : Super(ComponentType::Button)
{
}

Button::~Button()
{
}

void Button::Update()
{
    Super::Update();

    // Scene이 소멸 중이면 업데이트 중단
    if (CURSCENE->IsDestroying()) {
        return;
    }

    // GameObject 유효성 검사
    auto go = GetGameObject();
    if (!go) {
        return;
    }

    if (m_isEnabled)
    {
        UpdateState();
    }
}

bool Button::Picked(POINT _screenPos)
{


    return ::PtInRect(&m_rect, _screenPos);
}


void Button::Create(Vec2 _localPos, Vec2 _size, shared_ptr<class Material> _material, uint32 _pass)
{
    auto go = m_gameObject.lock();

    m_size = _size;
    m_defaultMaterial = _material;

    // Normal 상태의 기본 Material로 설정
    SetMaterial(ButtonState::Normal, _material);

    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();

    float x = _localPos.x - width / 2;
    float y = height / 2 - _localPos.y;

    Vec3 position = Vec3(x, y, m_zPos);

    go->GetTransform()->SetPosition(position);
    go->GetTransform()->SetScale(Vec3(_size.x * RESOLUTION_CONSTANT, _size.y * RESOLUTION_CONSTANT, 1));

    go->SetLayerIndex(LAYER_UI);

    if (go->GetMeshRenderer() == nullptr) {
        go->AddComponent(make_shared<MeshRenderer>());
    }

    go->GetMeshRenderer()->SetMaterial(_material);

    auto mesh = RESOURCES->Get<Mesh>(L"Quad");
    go->GetMeshRenderer()->SetMesh(mesh);
    go->GetMeshRenderer()->SetPass(_pass);



    m_materialSize = _material->GetDiffuseMap()->GetSize();

    //Picking;
    m_rect.left = static_cast<LONG>(_localPos.x - _size.x / 2.f);
    m_rect.right = static_cast<LONG>(_localPos.x + _size.x / 2.f);
    m_rect.top = static_cast<LONG>(_localPos.y - _size.y / 2.f);
    m_rect.bottom = static_cast<LONG>(_localPos.y + _size.y / 2.f);

    // 초기 상태 설정
    ChangeState(ButtonState::Normal);
}


//[](){} 람다로 넣어주면 문제가 생길 수 있음
//포인터 타입을 만약 넣어준다. 나중에 메모리가 해제되면 망할 수 있다. 
//스마트 포인터를 사용하면, 누구라도 한 명이라도 기억하지 않으면
//캡쳐 하는 바람에 영영 해제가 안되고 메모리 누수가 일어날 수 있음. 
void Button::AddOnClickedEvent(std::function<void(void)> _func)
{
    m_onClicked = _func;
}

void Button::InvokeOnClicked()
{
   /* if (m_onClicked)
        m_onClicked();*/

    OnClick();
}

// Button.cpp에 새로운 함수들 추가
void Button::UpdatePosition(const Vec2& parentWorldPos)
{
    auto go = m_gameObject.lock();
    if (!go) return;

    // 부모의 월드 위치 + 로컬 위치로 새 월드 위치 계산
    Vec2 newWorldPos;
    newWorldPos.x = parentWorldPos.x + m_localPosition.x;
    newWorldPos.y = parentWorldPos.y + m_localPosition.y;

    // 화면 좌표를 월드 좌표로 변환
    float height = GRAPHICS->GetViewport().GetHeight();
    float width = GRAPHICS->GetViewport().GetWidth();

    float x = newWorldPos.x - width / 2;
    float y = height / 2 - newWorldPos.y;

    
    go->GetTransform()->SetPosition(Vec3(x, y, m_zPos));

    // Picking RECT도 업데이트
    UpdatePickingRect(newWorldPos);
}

void Button::UpdatePickingRect(const Vec2& screenPos)
{
    m_rect.left = static_cast<LONG>(screenPos.x - m_size.x / 2.f);
    m_rect.right = static_cast<LONG>(screenPos.x + m_size.x / 2.f);
    m_rect.top = static_cast<LONG>(screenPos.y - m_size.y / 2.f);
    m_rect.bottom = static_cast<LONG>(screenPos.y + m_size.y / 2.f);
}


void Button::SetMaterial(ButtonState state, shared_ptr<class Material> material)
{
    m_stateMaterials[state] = material;

    // 현재 상태와 같다면 즉시 적용
    if (m_currentState == state)
    {
        ApplyCurrentMaterial();
    }
}

void Button::SetEnabled(bool enabled)
{
    if (m_isEnabled != enabled)
    {
        m_isEnabled = enabled;

        if (!enabled)
        {
            ChangeState(ButtonState::Disabled);
        }
        else
        {
            ChangeState(ButtonState::Normal);
        }
    }
}

void Button::UpdateState()
{
    //if (!m_isEnabled)
    //    return;

    //POINT mousePos = INPUT->GetMousePos();
    //bool isMouseInside = Picked(mousePos);
    //bool isMousePressed = INPUT->GetButton(KEY_TYPE::LBUTTON);
    //bool isMouseClicked = INPUT->GetButtonDown(KEY_TYPE::LBUTTON);

    //ButtonState newState = m_currentState;

    //// 마우스가 버튼 안에 있는 경우
    //if (isMouseInside)
    //{
    //    // 마우스가 처음 들어온 경우
    //    if (!m_isMouseInside)
    //    {
    //        OnHoverEnter();
    //    }

    //    // 마우스가 눌려진 상태
    //    if (isMousePressed)
    //    {
    //        newState = ButtonState::Pressed;
    //    }
    //    else
    //    {
    //        newState = ButtonState::Hovered;
    //    }

    //    // 클릭된 경우
    //    if (isMouseClicked)
    //    {
    //        OnClick();
    //    }
    //}
    //else
    //{
    //    // 마우스가 버튼 밖으로 나간 경우
    //    if (m_isMouseInside)
    //    {
    //        OnHoverExit();
    //    }

    //    newState = ButtonState::Normal;
    //}

    //// 상태 업데이트
    //m_isMouseInside = isMouseInside;
    //m_wasMousePressed = isMousePressed;

    //// 상태 변경
    //if (newState != m_currentState)
    //{
    //    ChangeState(newState);
    //}

    if (!m_isEnabled)
        return;

    POINT mousePos = INPUT->GetMousePos();
    bool isMouseInside = Picked(mousePos);
    bool isMousePressed = INPUT->GetButton(KEY_TYPE::LBUTTON);
    bool isMouseDown = INPUT->GetButtonDown(KEY_TYPE::LBUTTON);
    bool isMouseUp = INPUT->GetButtonUp(KEY_TYPE::LBUTTON);

    ButtonState newState = m_currentState;

    // 마우스가 버튼 안에 있는 경우
    if (isMouseInside)
    {
        // 마우스가 처음 들어온 경우
        if (!m_isMouseInside)
        {
            OnHoverEnter();
        }

        // 버튼 안에서 마우스를 누르기 시작한 경우
        if (isMouseDown)
        {
            m_clickStartedInside = true;
        }

        // 마우스가 눌려진 상태
        if (isMousePressed)
        {
            newState = ButtonState::Pressed;
        }
        else
        {
            newState = ButtonState::Hovered;
        }

        // 클릭 판정: 버튼 안에서 누르기 시작했고, 버튼 안에서 떼는 경우
        if (isMouseUp && m_clickStartedInside)
        {
            OnClick();
            m_clickStartedInside = false; // 클릭 완료 후 리셋
        }
    }
    else
    {
        // 마우스가 버튼 밖으로 나간 경우
        if (m_isMouseInside)
        {
            OnHoverExit();
        }

        newState = ButtonState::Normal;

        // 버튼 밖에서 마우스를 떼면 클릭 시작 플래그 리셋
        if (isMouseUp)
        {
            m_clickStartedInside = false;
        }
    }

    // 상태 업데이트
    m_isMouseInside = isMouseInside;
    m_wasMousePressed = isMousePressed;

    // 상태 변경
    if (newState != m_currentState)
    {
        ChangeState(newState);
    }
}

void Button::ChangeState(ButtonState newState)
{
    m_previousState = m_currentState;
    m_currentState = newState;

    ApplyCurrentMaterial();
    OnStateChanged(newState);
}

void Button::ApplyCurrentMaterial()
{
    // Scene 소멸 중이면 Material 변경 금지
    if (CURSCENE->IsDestroying()) {
        return;
    }

    auto go = GetGameObject();
    if (!go) {
        std::wcout << L"Button::ApplyCurrentMaterial - GameObject가 유효하지 않습니다!" << std::endl;
        return;
    }

    if (!go->GetMeshRenderer()) {
        std::wcout << L"Button::ApplyCurrentMaterial - MeshRenderer가 없습니다!" << std::endl;
        return;
    }

    shared_ptr<Material> materialToApply = nullptr;

    // 현재 상태에 맞는 Material 찾기
    auto it = m_stateMaterials.find(m_currentState);
    if (it != m_stateMaterials.end() && it->second != nullptr)
    {
        materialToApply = it->second;
    }
    else
    {
        // 해당 상태의 Material이 없으면 기본 Material 사용
        materialToApply = m_defaultMaterial;
    }

    if (materialToApply)
    {
        go->GetMeshRenderer()->SetMaterial(materialToApply);
    }
    else
    {
        std::cout << "Button::ApplyCurrentMaterial - 적용할 Material이 없습니다!" << std::endl;
    }
}
