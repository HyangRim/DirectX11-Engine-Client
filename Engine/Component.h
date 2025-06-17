#pragma once


class GameObject;
class Transform;

enum class ComponentType : uint8 {
	Transform,
	MeshRenderer,
	ModelRenderer,
	Camera,
	Animator,
	Light,
	// ...
	Script,
	End
};

enum {
	FIXED_COMPONENT_COUNT = static_cast<uint8>(ComponentType::End) - 1
};

class Component
{
public:
	Component(ComponentType _type);
	virtual ~Component();

	virtual void Init() {}
	virtual void Start() {}

	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}

public:
	ComponentType GetType() { return m_type; }


	//GameObject 가져오기. 
	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> _gameObject) { m_gameObject = _gameObject; }

protected:
	ComponentType m_type;
	//자신을 소유한 오브젝트. 
	weak_ptr<GameObject> m_gameObject;

};

