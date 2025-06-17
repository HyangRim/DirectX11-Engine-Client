#include "pch.h"
#include "Camera.h"
#include "Component.h"
#include "Transform.h"


Matrix Camera::s_MatView = Matrix::Identity;
Matrix Camera::s_MatProjection = Matrix::Identity;

Camera::Camera() : Super(ComponentType::Camera)
{
	m_width = static_cast<float>(GAME->GetGameDesc().width);
	m_height = static_cast<float>(GAME->GetGameDesc().height);
}

Camera::~Camera()
{
}

void Camera::Update()
{
	UpdateMatrix();

	RENDER->PushGlobalData(Camera::s_MatView, Camera::s_MatProjection);
}



//카메라랑 연관있는 건 View와 Projection.

//View는 당연히 카메라 좌표 기준에서 봐야하니까.
//Projection은 
void Camera::UpdateMatrix()
{
	Vec3 eyePosition = GetTransform()->GetPosition();
	Vec3 focusPosition = eyePosition + GetTransform()->GetLook();
	Vec3 upDirection = GetTransform()->GetUp();
	s_MatView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);


	if (m_type == ProjectionType::Perspective) {
		s_MatProjection = ::XMMatrixPerspectiveFovLH(m_fov, m_width / m_height, m_near, m_far);
	}
	else {
		s_MatProjection = ::XMMatrixOrthographicLH(1366.f, 768.f, 0.f, 1.f);
	}
}
