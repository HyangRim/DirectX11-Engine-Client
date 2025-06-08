#pragma once

#include "Component.h"
class Pipeline;
class MeshRenderer;
class Transform;
class MonoBehaviour;
class Camera;
class Animator;


class GameObject : public enable_shared_from_this<GameObject>
{

public:
	GameObject();
	virtual ~GameObject();

	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();

	shared_ptr<Component> GetFixedComponent(ComponentType _type);
	shared_ptr<Transform> GetTransform();
	shared_ptr<Camera> GetCamera();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Transform> GetOrAddTransform();
	//shared_ptr<Animator> GetAnimator();

	void AddComponent(shared_ptr<Component> _component);

protected:

	//고정된 배열. Component별 고정된 위치. 
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> m_components;

	//스트립트는 따로. 
	vector<shared_ptr<MonoBehaviour>> m_scripts;
};

