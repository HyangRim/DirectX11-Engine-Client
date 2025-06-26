#include "pch.h"
#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"
#include "GameObject.h"

SphereCollider::SphereCollider()
	:BaseCollider(ColliderType::Sphere)
{
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::Update()
{
	m_boundingSphere.Center = GetGameObject()->GetTransform()->GetPosition();

	Vec3 scale = GetGameObject()->GetTransform()->GetScale();
	m_boundingSphere.Radius = m_radius * max(max(scale.x, scale.y), scale.z);
}

bool SphereCollider::Intersects(Ray& _ray, OUT float& _distance)
{
	return m_boundingSphere.Intersects(_ray.position, _ray.direction, OUT _distance);
}

bool SphereCollider::Intersects(shared_ptr<BaseCollider>& _other)
{
	ColliderType type = _other->GetColliderType();

	switch (type) {
		case ColliderType::Sphere:
			return m_boundingSphere.Intersects(dynamic_pointer_cast<SphereCollider>(_other)->GetBoundSphere());
		case ColliderType::AABB:
			return m_boundingSphere.Intersects(dynamic_pointer_cast<AABBBoxCollider>(_other)->GetBoundingBox());
		case ColliderType::OBB:
			return m_boundingSphere.Intersects(dynamic_pointer_cast<OBBBoxCollider>(_other)->GetBoundingBox());
	}

	return false;
}
