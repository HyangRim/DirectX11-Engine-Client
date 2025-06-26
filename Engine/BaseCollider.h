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
	static uint32 s_nextID;


	BaseCollider(ColliderType _type);
	virtual ~BaseCollider();

	virtual bool Intersects(Ray& _ray, OUT float& _distance) = 0;
	virtual bool Intersects(shared_ptr<BaseCollider>& _other) = 0;


	ColliderType GetColliderType() { return m_colliderType; }

	uint32 GetID() { return m_id; }

	void SetActive(bool _active) { m_active == _active; }
	bool GetActive() { return m_active; }

protected:
	ColliderType m_colliderType;

	uint32 m_id;			//충돌체 고유한 ID값. 
	bool m_active = true;	//충돌체 활성화 여부. 
};

