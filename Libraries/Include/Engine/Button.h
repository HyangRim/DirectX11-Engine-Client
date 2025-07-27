#pragma once
#include "Component.h"
class Button : public Component
{
	using Super = Component;
public:
	Button();
	virtual ~Button();

	bool Picked(POINT _screenPos);
	void Create(Vec2 _screenPos, Vec2 _size, shared_ptr<class Material> _material, uint32 _pass = 0);

	void AddOnClickedEvent(std::function<void(void)> _func);
	void InvokeOnClicked();
private:

	std::function<void(void)> m_onClicked;
	RECT m_rect;
	uint32 m_pass;

	// Button.h에 추가
private:
	Vec2 m_localPosition;  // 부모(UIPanel) 기준 로컬 위치
	Vec2 m_size;           // 버튼 크기

public:
	void UpdatePosition(const Vec2& parentWorldPos);  // 부모 위치 기준으로 업데이트
	void SetLocalPosition(const Vec2& localPos) { m_localPosition = localPos; }
	const Vec2& GetLocalPosition() const { return m_localPosition; }
	void UpdatePickingRect(const Vec2& screenPos);
};

//인게임, 설계, 매니저등이 중요함
//버튼, 이펙트는 그리 중요하지 않음.

//코딩 난이도, 기술적인 거 포폴에 중요함. 