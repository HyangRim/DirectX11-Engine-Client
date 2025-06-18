#pragma once
#include "Component.h"

//Perspective  -> ���ٹ� �߰�
//Orthographic -> ��������. ���� ������ ������ �� �Ȱ��� ũ��� ���� 
//UI, 2D������ Orthographic�� ����. 

enum class ProjectionType {
    Perspective,
    Orthographic
};
class Camera :
    public Component
{
    using Super = Component;

public:
    Camera();
    virtual ~Camera();

    virtual void Update() override;

    void SetProjectionType(ProjectionType _type) { m_type = _type; }
    ProjectionType GetProjectionType() { return m_type; }

    void UpdateMatrix();
    void SetNear(float _value) { m_near = _value; }
    void SetFar(float _value) { m_far = _value; }
    void SetFOV(float _value) { m_fov = _value; }
    void SetWidth(float _value) { m_width = _value; }
    void SetHeight(float _value) { m_height = _value; }

    Matrix& GetViewMatrix() { return m_matView; }
    Matrix& GetProjectionMatrix() { return m_matProjection; }

private:
    ProjectionType m_type = ProjectionType::Perspective;

    Matrix m_matView = Matrix::Identity;
    Matrix m_matProjection = Matrix::Identity;

    float m_near = 1.f;
    float m_far = 1000.f;
    float m_fov = XM_PI / 4.f;
    float m_width = 0.f;
    float m_height = 0.f;

public:
    static Matrix s_MatView;
    static Matrix s_MatProjection;


};

