#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"

Component::Component(ComponentType _type)
	: m_type(_type)
{
}

Component::~Component()
{
}



shared_ptr<GameObject> Component::GetGameObject()
{
	return m_gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	return m_gameObject.lock()->GetTransform();
}
