#include "pch.h"
#include "SphereCollider.h"

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
