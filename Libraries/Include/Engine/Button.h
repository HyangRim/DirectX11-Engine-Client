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

//인게임, 설계, 매니저등이 중요함
//버튼, 이펙트는 그리 중요하지 않음.

//코딩 난이도, 기술적인 거 포폴에 중요함. 