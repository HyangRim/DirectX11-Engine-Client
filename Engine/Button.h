#pragma once
#include "Component.h"
#include "Delegate.h"

enum class ButtonState
{
	Normal,
	Hovered,
	Pressed,
	Disabled
};

class Button : public Component
{
	using Super = Component;
public:
	Button();
	virtual ~Button();

	virtual void Update() override;

	bool Picked(POINT _screenPos);
	void Create(Vec2 _localPos, Vec2 _size, shared_ptr<class Material> _material, uint32 _pass = 0);

	//기존 이벤트
	void AddOnClickedEvent(std::function<void(void)> _func);
	void InvokeOnClicked();

	// 새로운 이벤트들 (Delegate 사용)
	Delegate::Delegate<> OnClick;
	Delegate::Delegate<> OnHoverEnter;
	Delegate::Delegate<> OnHoverExit;
	Delegate::Delegate<ButtonState> OnStateChanged;

	// Material 설정 함수들
	void SetMaterial(ButtonState state, shared_ptr<class Material> material);
	void SetNormalMaterial(shared_ptr<class Material> material) { SetMaterial(ButtonState::Normal, material); }
	void SetHoveredMaterial(shared_ptr<class Material> material) { SetMaterial(ButtonState::Hovered, material); }
	void SetPressedMaterial(shared_ptr<class Material> material) { SetMaterial(ButtonState::Pressed, material); }
	void SetDisabledMaterial(shared_ptr<class Material> material) { SetMaterial(ButtonState::Disabled, material); }

	// 상태 관리
	ButtonState GetCurrentState() const { return m_currentState; }
	void SetEnabled(bool enabled);
	bool IsEnabled() const { return m_isEnabled; }

	//위치관련
	void UpdatePosition(const Vec2& parentWorldPos);  // 부모 위치 기준으로 업데이트
	void SetLocalPosition(const Vec2& localPos) { m_localPosition = localPos; }
	const Vec2& GetLocalPosition() const { return m_localPosition; }
	void UpdatePickingRect(const Vec2& screenPos);
	void SetZPos(float zPos) { m_zPos = zPos; }

	RECT GetRect() { return m_rect; }

private:
	void UpdateState();
	void ChangeState(ButtonState newState);
	void ApplyCurrentMaterial();


private:

	std::function<void(void)> m_onClicked;
	RECT m_rect;
	uint32 m_pass;
	float m_zPos = 0.6f;

	Vec2 m_localPosition;  // 부모(UIPanel) 기준 로컬 위치
	Vec2 m_size;           // 버튼 크기
	Vec2 m_materialSize;

	bool m_clickStartedInside = false;

	// 상태 관리
	ButtonState m_currentState = ButtonState::Normal;
	ButtonState m_previousState = ButtonState::Normal;
	bool m_isEnabled = true;
	bool m_isMouseInside = false;
	bool m_wasMousePressed = false;


	// Material 관리
	std::map<ButtonState, shared_ptr<class Material>> m_stateMaterials;
	shared_ptr<class Material> m_defaultMaterial = nullptr;


};

//인게임, 설계, 매니저등이 중요함
//버튼, 이펙트는 그리 중요하지 않음.

//코딩 난이도, 기술적인 거 포폴에 중요함. #include "pch.h"