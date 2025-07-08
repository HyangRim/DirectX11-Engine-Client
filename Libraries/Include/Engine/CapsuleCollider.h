#pragma once
#include "BaseCollider.h"
#include "GameObject.h"

struct CapsuleGeometry {
    Vec3 center;        // Ä¸½¶ÀÇ Áß½ÉÁ¡
    float radius;       // Ä¸½¶ÀÇ ¹ÝÁö¸§
    float height;       // Ä¸½¶ÀÇ ÀüÃ¼ ³ôÀÌ
    int direction;      // 0=XÃà, 1=YÃà, 2=ZÃà

    // Ä¸½¶ÀÇ µÎ ³¡Á¡ °è»ê
    Vec3 GetTopPoint() const {
        Vec3 offset = Vec3::Zero;
        float halfHeight = (height - 2.0f * radius) * 0.5f;
        offset[direction] = halfHeight;
        return center + offset;
    }

    Vec3 GetBottomPoint() const {
        Vec3 offset = Vec3::Zero;
        float halfHeight = (height - 2.0f * radius) * 0.5f;
        offset[direction] = -halfHeight;
        return center + offset;
    }
};

class CapsuleCollider : public BaseCollider
{
public:
    CapsuleCollider();
    virtual ~CapsuleCollider();

    virtual void Update() override;
    virtual bool Intersects(Ray& _ray, OUT float& _distance) override;
    virtual bool Intersects(shared_ptr<BaseCollider>& _other) override;

    // Ä¸½¶ ¼Ó¼º ¼³Á¤/Á¶È¸
    void SetCenter(const Vec3& _center) { m_capsule.center = _center; }
    void SetRadius(float _radius) { m_capsule.radius = _radius; }
    void SetHeight(float _height) { m_capsule.height = _height; }
    void SetDirection(int _direction) { m_capsule.direction = _direction; }

    Vec3 GetCenter() const { return m_capsule.center; }
    float GetRadius() const { return m_capsule.radius; }
    float GetHeight() const { return m_capsule.height; }
    int GetDirection() const { return m_capsule.direction; }

    const CapsuleGeometry& GetCapsule() const { return m_capsule; }

private:
    CapsuleGeometry m_capsule;
    shared_ptr<GameObject> m_DebugObject;

    // À¯Æ¿¸®Æ¼ ÇÔ¼öµé
    float DistancePointToLineSegment(const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd) const;
    bool IntersectsCapsuleSphere(const CapsuleGeometry& capsule, const BoundingSphere& sphere) const;
    bool IntersectsCapsuleAABB(const CapsuleGeometry& capsule, const BoundingBox& aabb) const;
    bool IntersectsCapsuleCapsule(const CapsuleGeometry& capsule1, const CapsuleGeometry& capsule2) const;
};
