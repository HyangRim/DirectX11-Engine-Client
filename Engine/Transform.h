#pragma once
#include "Component.h"
class Transform :
    public Component
{
	using Super = Component;
public:
    Transform();
    virtual ~Transform();

    virtual void Init() override;
    virtual void Update() override;


	void UpdateTransform();

	//Local
	Vec3 GetLocalScale() { return m_localScale; }
	void SetLocalScale(const Vec3& _localScale) { m_localScale = _localScale; UpdateTransform(); }
	
	Vec3 GetLocalRotation() { return m_localRotation; }
	void SetLocalRotation(const Vec3& _localRotation) { m_localRotation = _localRotation; UpdateTransform(); }
	
	Vec3 GetLocalPosition() { return m_localPosition; }
	void SetLocalPosition(const Vec3& _localPosition) { m_localPosition = _localPosition; UpdateTransform(); }


	//World
	Vec3 GetScale() { return m_WorldScale; }
	void SetScale(const Vec3& _Scale);

	Vec3 GetRotation() { return m_WorldRotation; }
	void SetRotation(const Vec3& _Rotation);

	Vec3 GetPosition() { return m_WorldPosition; }
	void SetPosition(const Vec3& _Position);

	Vec3 GetRight() { return m_matWorld.Right(); }
	Vec3 GetUp() { return m_matWorld.Up(); }
	Vec3 GetLook() { return m_matWorld.Backward(); }

	Matrix GetWorldMatrix() { return m_matWorld; }

	bool HasParent() { return  m_parent != nullptr; }
	shared_ptr<Transform> GetParent() { return m_parent; }
	void SetParent(shared_ptr<Transform> _parent) { m_parent = _parent; }

	const vector<shared_ptr<Transform>>& GetChildren() { return m_children; }
	void AddChild(shared_ptr<Transform> _child) {
		m_children.emplace_back(_child);
	}
	
	static Vec3 ToEulerAngles(Quaternion q);

private:

	//스 자 이 공 부 
	//스케일 -> 자전 -> 이동 -> 공전 -> 부모 순으로 연산. 
	Vec3 m_localScale = { 1.f, 1.f, 1.f };
	Vec3 m_localRotation = { 0.f, 0.f, 0.f };
	Vec3 m_localPosition = { 0.f, 0.f, 0.f };


	//나의 부모를 좌표계로 삼는 local 좌표계. 
	Matrix m_matLocal = Matrix::Identity;
	Matrix m_matWorld = Matrix::Identity;


	//Cache
	Vec3 m_WorldScale;
	Vec3 m_WorldRotation;
	Vec3 m_WorldPosition;

private:
	shared_ptr<Transform> m_parent;
	vector<shared_ptr<Transform>> m_children;
};

