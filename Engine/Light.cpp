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

    //  라이트 방향을 정규화된 Direction에서 가져오기
    Vec3 lightDir = Vec3(m_desc.direction.x, m_desc.direction.y, m_desc.direction.z);
    lightDir.Normalize();

    //  라이트 위치를 더 명확하게 계산
    Vec3 eyePosition = camPos + (-lightDir * _backDist);
    Vec3 focusPosition = camPos; // 카메라 위치를 바라보도록
    Vec3 upDirection = Vec3::Up;

    //  라이트가 거의 수직일 때 Up 벡터 조정
    if (abs(lightDir.y) > 0.95f) {
        upDirection = Vec3::Right;
    }

    s_MatView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
    s_MatProjection = _matProjection;

    Matrix T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    s_ShadowTransform = s_MatView * s_MatProjection * T;

    //  더 자세한 디버깅 출력
    char debugStr[1024];
    sprintf_s(debugStr, "=== Shadow Matrix Debug ===\n"
        "Light Dir: (%.3f, %.3f, %.3f)\n"
        "Eye Pos: (%.3f, %.3f, %.3f)\n"
        "Focus Pos: (%.3f, %.3f, %.3f)\n"
        "View Matrix: [%.3f, %.3f, %.3f, %.3f]\n"
        "Proj Matrix: [%.3f, %.3f, %.3f, %.3f]\n"
        "Shadow Transform: [%.3f, %.3f, %.3f, %.3f]\n",
        lightDir.x, lightDir.y, lightDir.z,
        eyePosition.x, eyePosition.y, eyePosition.z,
        focusPosition.x, focusPosition.y, focusPosition.z,
        s_MatView._11, s_MatView._12, s_MatView._13, s_MatView._14,
        s_MatProjection._11, s_MatProjection._12, s_MatProjection._13, s_MatProjection._14,
        s_ShadowTransform._11, s_ShadowTransform._12, s_ShadowTransform._13, s_ShadowTransform._14);
    cout << debugStr << "\n";
}
