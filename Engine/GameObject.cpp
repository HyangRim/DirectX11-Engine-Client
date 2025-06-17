#include "pch.h"
#include "GameObject.h"

#include "Transform.h"
#include "Component.h"
#include "MonoBehaviour.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "Camera.h"
//#include "Animator.h"
//#include "Animation.h"
#include "ModelAnimation.h"
#include "ModelAnimator.h"
#include "Light.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{
}

void GameObject::Init()
{
	for (shared_ptr<Component>& component : m_components) {
		if(component)
			component->Init();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts) {
		if (script)
			script->Init();
	}
}

void GameObject::Start()
{
	for (shared_ptr<Component>& component : m_components) {
		if (component)
			component->Start();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts) {
		if (script)
			script->Start();
	}
}

void GameObject::Update()
{
	for (shared_ptr<Component>& component : m_components) {
		if(component != nullptr)
			component->Update();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts) {
		if (script)
			script->Update();
	}
}

void GameObject::LateUpdate()
{
	for (shared_ptr<Component>& component : m_components) {
		if (component != nullptr)
			component->LateUpdate();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts) {
		script->LateUpdate();
	}
}

void GameObject::FixedUpdate()
{
	for (shared_ptr<Component>& component : m_components) {
		if (component != nullptr)
			component->FixedUpdate();
	}

	for (shared_ptr<MonoBehaviour>& script : m_scripts) {
		script->FixedUpdate();
	}
}

shared_ptr<Component> GameObject::GetFixedComponent(ComponentType _type)
{

	uint8 index = static_cast<uint8>(_type);
	assert(index < FIXED_COMPONENT_COUNT);
	return m_components[index];
}

shared_ptr<Transform> GameObject::GetTransform()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Transform);
	return static_pointer_cast<Transform>(component);
}

shared_ptr<Camera> GameObject::GetCamera()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Camera);
	return static_pointer_cast<Camera>(component);
}

shared_ptr<MeshRenderer> GameObject::GetMeshRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::MeshRenderer);
	return static_pointer_cast<MeshRenderer>(component);
}

shared_ptr<ModelRenderer> GameObject::GetModelRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::ModelRenderer);
	return static_pointer_cast<ModelRenderer>(component);
}

shared_ptr<Transform> GameObject::GetOrAddTransform()
{
	if (GetTransform() == nullptr) {
		shared_ptr<Transform> transform = make_shared<Transform>();
		AddComponent(transform);
	}

	return GetTransform();
}

shared_ptr<ModelAnimator> GameObject::GetModelAnimator()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Animator);
	return static_pointer_cast<ModelAnimator>(component);
}

shared_ptr<Light> GameObject::GetLight()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Light);
	return static_pointer_cast<Light>(component);
}


void GameObject::AddComponent(shared_ptr<Component> _component)
{
	//enable_shared_from_this �Ⱦ���, this�� �׳� �Ѱ��ָ�. 
	//���۷��� ī��Ʈ�� �������� �����ؼ� �ȵ�. 
	_component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(_component->GetType());

	if (index < FIXED_COMPONENT_COUNT) {
		m_components[index] = _component;
	}
	else {
		m_scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(_component));
	}
}

