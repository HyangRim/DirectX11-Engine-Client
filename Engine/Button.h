#pragma once
#include "Component.h"
class Button : public Component
{
	using Super = Component;
public:
	Button();
	virtual ~Button();

	bool Picked(POINT _screenPos);
	void Create(Vec2 _screenPos, Vec2 _size, shared_ptr<class Material> _material);

	void AddOnClikedEvent(std::function<void(void)> _func);
	void InvokeOnClicked();
private:

	std::function<void(void)> m_onClicked;
	RECT m_rect;
};

//�ΰ���, ����, �Ŵ������� �߿���
//��ư, ����Ʈ�� �׸� �߿����� ����.

//�ڵ� ���̵�, ������� �� ������ �߿���. 