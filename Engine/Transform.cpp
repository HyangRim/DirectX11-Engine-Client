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

	//부모가 없으면 local행렬이 world와 같음. 

	//부모가 있을 때는 부모 좌표계를 기준으로 변환. 
	if (HasParent()) {
		m_matWorld = m_matLocal * m_parent->GetWorldMatrix();
	}
	else {
		m_matWorld = m_matLocal;
	}

	//여기서 right, up, look 벡터는 무엇인가??
	Quaternion quat;
	m_matWorld.Decompose(m_WorldScale, quat, m_WorldPosition);
	m_WorldRotation = ToEulerAngles(quat);


	//Coord와 Normal방식. 
	//방향만 바꾸고 싶다. 면, (1,0, 0, 1) <- 4번째가 0이냐 1이냐 차이. 




	//Children들 Transform변경
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
		//World -> Parent좌표계로 변경. 
		Matrix worldToParentLocalMatrix = m_parent->GetWorldMatrix().Invert();
		Vec3 position;

		position.Transform(m_WorldPosition, worldToParentLocalMatrix);
		SetLocalPosition(position);
	}
	else {
		SetLocalPosition(_Position);
	}
}


