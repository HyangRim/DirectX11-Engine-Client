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

	virtual bool Intersects(Ray& _ray, OUT float& _distance) = 0;
	virtual bool Intersects(shared_ptr<BaseCollider>& _other) = 0;


	ColliderType GetColliderType() { return m_colliderType; }

protected:
	ColliderType m_colliderType;
};

