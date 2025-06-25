#include "pch.h"
#include "Light.h"
#include "Camera.h"

Matrix Light::s_MatView = Matrix::Identity;
Matrix Light::s_MatProjection = Matrix::Identity;
Matrix Light::s_ShadowTransform = Matrix::Identity;

Light::Light() : Component(ComponentType::Light)
{
}

Light::~Light()
{
}

void Light::Update()
{
	//RENDER->PushLightData(m_desc);
}

void Light::SetVPMatrix(Camera* _camera, float _backDist, Matrix _matProjection)
{
	Vec3 camPos = _camera->GetTransform()->GetPosition();
	Vec3 lookVec = GetTransform()->GetLook();

	Vec3 eyePosition = camPos - lookVec * _backDist;
	Vec3 focusPosition = eyePosition + lookVec;
	Vec3 upDirection = Vec3::Up;

	s_MatView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	s_MatProjection = _matProjection;

	Matrix T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	s_ShadowTransform = s_MatView * s_MatProjection * T;
}
