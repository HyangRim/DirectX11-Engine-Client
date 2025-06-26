#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "Rigidbody.h"

Component::Component(ComponentType _type)
	: m_type(_type)
{
}

Component::~Component()
{
}



shared_ptr<GameObject> Component::GetGameObject()
{
	//lock�� �ؼ� �ְ� �Ǹ� weak_ptr�� �޴´�? 
	//lock�� ������ ��ü�� ���� ����ִٸ� ��ȿ�� shared_ptr��ȯ
	//���� ��ü�� �ı��Ǿ� ������ nullptr�� ��ȯ�Ѵ�. 
	//lock�� weak_ptr���� ���� ��ü�� �����ϰ� ���� �� ����ϴ� �Լ�.
	
	//��ü�� ������ ���� �����ϰ� �����Ϸ���. 
	return m_gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	return m_gameObject.lock()->GetTransform();
}

shared_ptr<Rigidbody> Component::GetRigidbody()
{
	return m_gameObject.lock()->GetRigidbody();
}
