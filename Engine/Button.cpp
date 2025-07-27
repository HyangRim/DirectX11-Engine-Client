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

bool Button::Picked(POINT _screenPos)
{
	return ::PtInRect(&m_rect, _screenPos);
}


void Button::Create(Vec2 _screenPos, Vec2 _size, shared_ptr<class Material> _material, uint32 _pass)
{
	auto go = m_gameObject.lock();

	m_size = _size;
	
	float height = GRAPHICS->GetViewport().GetHeight();
	float width = GRAPHICS->GetViewport().GetWidth();

	float x = _screenPos.x - width / 2;
	float y = height / 2 - _screenPos.y;

	Vec3 position = Vec3(x, y, 0.5f);

	go->GetTransform()->SetPosition(position);
	go->GetTransform()->SetScale(Vec3(_size.x, _size.y, 1));

	go->SetLayerIndex(LAYER_UI);

	if (go->GetMeshRenderer() == nullptr) {
		go->AddComponent(make_shared<MeshRenderer>());
	}

	go->GetMeshRenderer()->SetMaterial(_material);
	
	auto mesh = RESOURCES->Get<Mesh>(L"Quad");
	go->GetMeshRenderer()->SetMesh(mesh);
	go->GetMeshRenderer()->SetPass(_pass);

	//Picking;
	m_rect.left = static_cast<LONG>(_screenPos.x - _size.x / 2.f);
	m_rect.right = static_cast<LONG>(_screenPos.x + _size.x / 2.f);
	m_rect.top = static_cast<LONG>(_screenPos.y - _size.y / 2.f);
	m_rect.bottom = static_cast<LONG>(_screenPos.y + _size.y / 2.f);
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
	if (m_onClicked)
		m_onClicked();
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

	go->GetTransform()->SetPosition(Vec3(x, y, 0.5f));

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