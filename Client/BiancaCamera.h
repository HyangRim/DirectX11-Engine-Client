#pragma once
#include "MonoBehaviour.h"
class BiancaCamera :
    public MonoBehaviour
{
    using Super = Component;
public:
    BiancaCamera();
    virtual ~BiancaCamera() = default;

    virtual void Update() override;

    void SetTarget(shared_ptr<GameObject> _target) { m_target = _target; }
    void SetOffset(Vec3 _offset) { m_offset = _offset; }

private:
    weak_ptr<GameObject> m_target;
    Vec3 m_offset = Vec3(25.f, 27.f, -20.f);
    Vec3 m_rotationOffset = Vec3(20.f, -0.f, 0.f);
};

