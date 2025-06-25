#include "pch.h"
#include "Button.h"
#include "MeshRenderer.h"
#include "Material.h"

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

void Button::Create(Vec2 _screenPos, Vec2 _size, shared_ptr<class Material> _material)
{
	auto go = m_gameObject.lock();

	float height = GRAPHICS->GetViewport().GetHeight();
	float width = GRAPHICS->GetViewport().GetWidth();

	float x = _screenPos.x - width / 2;
	float y = height / 2 - _screenPos.y;

	Vec3 position = Vec3(x, y, 0);

	go->GetTransform()->SetPosition(position);
	go->GetTransform()->SetScale(Vec3(_size.x, _size.y, 1));

	go->SetLayerIndex(LAYER_UI);

	if (go->GetMeshRenderer() == nullptr) {
		go->AddComponent(make_shared<MeshRenderer>());
	}

	go->GetMeshRenderer()->SetMaterial(_material);
	
	auto mesh = RESOURCES->Get<Mesh>(L"Quad");
	go->GetMeshRenderer()->SetMesh(mesh);
	go->GetMeshRenderer()->SetPass(0);

	//Picking;
	m_rect.left = _screenPos.x - width / 2;
	m_rect.right = _screenPos.x + width / 2;

	m_rect.top = _screenPos.y - height / 2;
	m_rect.bottom = _screenPos.y + height / 2;
}


//[](){} 람다로 넣어주면 문제가 생길 수 있음
//포인터 타입을 만약 넣어준다. 나중에 메모리가 해제되면 망할 수 있다. 
//스마트 포인터를 사용하면, 누구라도 한 명이라도 기억하지 않으면
//캡쳐 하는 바람에 영영 해제가 안되고 메모리 누수가 일어날 수 있음. 
void Button::AddOnClikedEvent(std::function<void(void)> _func)
{
	m_onClicked = _func;
}

void Button::InvokeOnClicked()
{
	if (m_onClicked)
		m_onClicked();
}
