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


//[](){} ���ٷ� �־��ָ� ������ ���� �� ����
//������ Ÿ���� ���� �־��ش�. ���߿� �޸𸮰� �����Ǹ� ���� �� �ִ�. 
//����Ʈ �����͸� ����ϸ�, ������ �� ���̶� ������� ������
//ĸ�� �ϴ� �ٶ��� ���� ������ �ȵǰ� �޸� ������ �Ͼ �� ����. 
void Button::AddOnClikedEvent(std::function<void(void)> _func)
{
	m_onClicked = _func;
}

void Button::InvokeOnClicked()
{
	if (m_onClicked)
		m_onClicked();
}
