#pragma once
#include "Component.h"

enum class ColliderType {
	Sphere,
	AABB,
	OBB,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType _type);
	virtual ~BaseCollider();

	virtual bool Intersects(Ray& ray, OUT float& distance) = 0;


	ColliderType GetColliderType() { return m_colliderType; }

protected:
	ColliderType m_colliderType;
};

