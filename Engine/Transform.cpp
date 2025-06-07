#include "pch.h"
#include "Transform.h"

Transform::Transform() : Super(ComponentType::Transform)
{
}

Transform::~Transform()
{
}

Vec3 ToEulerAngles(Quaternion q)
{
	Vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	angles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

void Transform::Init()
{
}

void Transform::Update()
{

}

void Transform::UpdateTransform()
{
	//Scale Rotation Translation
	Matrix matScale = Matrix::CreateScale(m_localScale);
	Matrix matRotation = Matrix::CreateRotationX(m_localRotation.x);
	matRotation *= Matrix::CreateRotationY(m_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(m_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(m_localPosition);

	m_matLocal = matScale * matRotation * matTranslation;

	//�θ� ������ local����� world�� ����. 

	//�θ� ���� ���� �θ� ��ǥ�踦 �������� ��ȯ. 
	if (HasParent()) {
		m_matWorld = m_matLocal * m_parent->GetWorldMatrix();
	}
	else {
		m_matWorld = m_matLocal;
	}

	//���⼭ right, up, look ���ʹ� �����ΰ�??
	Quaternion quat;
	m_matWorld.Decompose(m_WorldScale, quat, m_WorldPosition);
	m_WorldRotation = ToEulerAngles(quat);


	//Coord�� Normal���. 
	//���⸸ �ٲٰ� �ʹ�. ��, (1,0, 0, 1) <- 4��°�� 0�̳� 1�̳� ����. 




	//Children�� Transform����
	for (const shared_ptr<Transform>& child : m_children) {
		child->UpdateTransform();
	}
}

void Transform::SetScale(const Vec3& _Scale)
{
	if (HasParent()) {
		Vec3 parentScale = m_parent->GetScale();
		Vec3 scale = m_WorldScale;

		scale.x /= parentScale.x;
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;
		SetLocalScale(scale);
	}
	else {
		SetLocalScale(_Scale);
	}
}

void Transform::SetRotation(const Vec3& _Rotation)
{
	if (HasParent()) {
		Matrix worldToParentLocalMatrix = m_parent->GetWorldMatrix().Invert();
		Vec3 rotation;

		rotation.Transform(m_WorldRotation, worldToParentLocalMatrix);
		SetLocalPosition(rotation);
	}
	else {
		SetLocalRotation(_Rotation);
	}
}

void Transform::SetPosition(const Vec3& _Position)
{
	if (HasParent()) {
		//World -> Parent��ǥ��� ����. 
		Matrix worldToParentLocalMatrix = m_parent->GetWorldMatrix().Invert();
		Vec3 position;

		position.Transform(m_WorldPosition, worldToParentLocalMatrix);
		SetLocalPosition(position);
	}
	else {
		SetLocalPosition(_Position);
	}
}


