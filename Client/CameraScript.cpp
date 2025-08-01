#include "pch.h"
#include "CameraScript.h"
#include "Transform.h"
#include "GameObject.h"

void CameraScript::Init()
{
}

void CameraScript::Update()
{
	float dt = TIME->GetDeltaTime();

	Vec3 pos = GetTransform()->GetPosition();

	if (INPUT->GetButton(KEY_TYPE::T)) {
		pos += GetTransform()->GetLook() * m_speed * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::F)) {
		pos -= GetTransform()->GetLook() * m_speed * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::G)) {
		pos -= GetTransform()->GetRight() * m_speed * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::H)) {
		pos += GetTransform()->GetRight() * m_speed * dt;
	}
	GetTransform()->SetPosition(pos);


	if (INPUT->GetButton(KEY_TYPE::Q))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += dt * 30.f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::E))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= dt * 30.f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += dt * 30.f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::C))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= dt * 30.f;
		GetTransform()->SetLocalRotation(rotation);
	}

}
