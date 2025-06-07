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

	if (INPUT->GetButton(KEY_TYPE::W)) {
		pos += GetTransform()->GetLook() * m_speed * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::S)) {
		pos -= GetTransform()->GetLook() * m_speed * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::A)) {
		pos -= GetTransform()->GetRight() * m_speed * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::D)) {
		pos += GetTransform()->GetRight() * m_speed * dt;
	}
	GetTransform()->SetPosition(pos);


	if (INPUT->GetButton(KEY_TYPE::Q)) {
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += dt * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

}
