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
class Rigidbody;
class Text;


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
	shared_ptr<Rigidbody> GetRigidbody();
	shared_ptr<Text> GetText();

	void AddComponent(shared_ptr<Component> _component);

	void SetLayerIndex(uint8 _layer) { m_layerIndex = _layer; }
	uint8 GetLayerIndex() { return m_layerIndex; }

	template<typename T>
	shared_ptr<T> GetFixedComponent(ComponentType _type) {
		return static_pointer_cast<T>(GetFixedComponent(_type));
	}

	vector<shared_ptr<MonoBehaviour>>& GetScripts() { return m_scripts; }

	//Collision 관련
	virtual void OnCollision(shared_ptr<GameObject> _other);
	virtual void OnCollisionEnter(shared_ptr<GameObject> _other);
	virtual void OnCollisionExit(shared_ptr<GameObject> _other);


	//FOW관련
	void SetAlpha(float _alpha) {
		if (abs(m_alpha - _alpha) > 0.01f) {
			m_alpha = _alpha;
			m_alphaChanged = true;
		}
	}
	float GetAlpha() const { return m_alpha; }
	bool HasAlphaChanged() const { return m_alphaChanged; }
	void ResetAlphaChanged() { m_alphaChanged = false; }

public:
	void SetName(wstring _name) { m_Name = _name; }
	wstring GetName() { return m_Name; }

protected:
	wstring m_Name;

	//고정된 배열. Component별 고정된 위치. 
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> m_components;

	//스트립트는 따로. 
	vector<shared_ptr<MonoBehaviour>> m_scripts;

	uint8 m_layerIndex = 0;


	
private:
	//FOW관련 코드. 
	float m_alpha = 1.0f;
	bool m_alphaChanged = false;

};

