#pragma once

#include "Component.h"
class Pipeline;
class MeshRenderer;
class ModelRenderer;
class Transform;
class MonoBehaviour;
class Camera;
class Animator;
class ModelAnimator;
class Light;
class BaseCollider;
class Terrain;
class Button;
class Billboard;
class SnowBillboard;
class ParticleSystem;
class Renderer;


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
	shared_ptr<ModelRenderer> GetModelRenderer();
	shared_ptr<ModelAnimator> GetModelAnimator();
	shared_ptr<Renderer> GetRenderer();
	shared_ptr<Light> GetLight();
	shared_ptr<BaseCollider> GetCollider();
	shared_ptr<Terrain> GetTerrain();
	shared_ptr<Button> GetButton();
	shared_ptr<Billboard> GetBillboard();
	shared_ptr<SnowBillboard> GetSnowBillboard();
	shared_ptr<ParticleSystem> GetParticleSystem();

	void AddComponent(shared_ptr<Component> _component);

	void SetLayerIndex(uint8 _layer) { m_layerIndex = _layer; }
	uint8 GetLayerIndex() { return m_layerIndex; }

	template<typename T>
	shared_ptr<T> GetFixedComponent(ComponentType _type) {
		return static_pointer_cast<T>(GetFixedComponent(_type));
	}
protected:

	//고정된 배열. Component별 고정된 위치. 
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> m_components;

	//스트립트는 따로. 
	vector<shared_ptr<MonoBehaviour>> m_scripts;


	uint8 m_layerIndex = 0;
};

