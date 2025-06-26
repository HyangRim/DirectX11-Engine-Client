#include "pch.h"
#include "OBBBoxCollider.h"
#include "AABBBoxCollider.h"
#include "SphereCollider.h"

OBBBoxCollider::OBBBoxCollider()
	: BaseCollider(ColliderType::OBB)
{
}

OBBBoxCollider::~OBBBoxCollider()
{
}

//부모를 따라감. 
void OBBBoxCollider::Update()
{
	m_boundingBox.Center = GetTransform()->GetPosition();
	m_boundingBox.Extents = GetTransform()->GetScale() * 0.5f;
}


//빛이나 다른 무언가에 부딪혔으면 true반환. 
bool OBBBoxCollider::Intersects(Ray& _ray, OUT float& _distance)
{
	return m_boundingBox.Intersects(_ray.position, _ray.direction, OUT _distance);
}

bool OBBBoxCollider::Intersects(shared_ptr<BaseCollider>& _other)
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
