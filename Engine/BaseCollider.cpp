#include "pch.h"
#include "BaseCollider.h"

BaseCollider::BaseCollider(ColliderType _type) 
	: Component(ComponentType::Collider), m_colliderType(_type)
{
}

BaseCollider::~BaseCollider()
{
}
