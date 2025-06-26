#include "pch.h"
#include "GameObject.h"
#include "AABBBoxCollider.h"
#include "SphereCollider.h"
#include "OBBBoxCollider.h"

AABBBoxCollider::AABBBoxCollider()
	: BaseCollider(ColliderType::AABB)
{
}

AABBBoxCollider::~AABBBoxCollider()
{
}

void AABBBoxCollider::Update()
{
}

bool AABBBoxCollider::Intersects(Ray& _ray, OUT float& _distance)
{
	return m_boundingBox.Intersects(_ray.position, _ray.direction, OUT _distance);
}

bool AABBBoxCollider::Intersects(shared_ptr<BaseCollider>& _other)
{
	ColliderType type = _other->GetColliderType();

	switch (type) {
	case ColliderType::Sphere:
		return m_boundingBox.Intersects(dynamic_pointer_cast<SphereCollider>(_other)->GetBoundSphere());
	case ColliderType::AABB:
		return m_boundingBox.Intersects(dynamic_pointer_cast<AABBBoxCollider>(_other)->GetBoundingBox());
	case ColliderType::OBB:
		return m_boundingBox.Intersects(dynamic_pointer_cast<OBBBoxCollider>(_other)->GetBoundingBox());
	}

	return false;
}
