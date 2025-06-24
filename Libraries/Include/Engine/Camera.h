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

    virtual void LateUpdate() override;

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
    
    float GetWidth() { return m_width;}
    float GetHeight() { return m_height; }

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


public:
    void SortGameObject();
    void Render_Forward();
    void Render_Backward();

    void SetCullingMaskLayerOnOff(uint8 _layer, bool _on) {
        if (_on) {
            m_cullingMask |= (1 << _layer);
        }
        else
            m_cullingMask &= ~(1 << _layer);
    }
    void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }
    void SetCullingMask(uint32 _mask) { m_cullingMask = _mask; }
    bool IsCulled(uint8 _layer) { return (m_cullingMask & (1 << _layer)) != 0; }

private:
    uint32 m_cullingMask = 0;
    vector<shared_ptr<GameObject>> m_vecForward;
    vector<shared_ptr<GameObject>> m_vecBackward;
};

