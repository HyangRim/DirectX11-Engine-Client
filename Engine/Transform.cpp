#include "pch.h"
#include "Transform.h"
#include "SimpleMath.h"

Transform::Transform() : Super(ComponentType::Transform)
{
}

Transform::~Transform()
{
}

Vec3 Transform::ToEulerAngles(Quaternion q)
{
	Vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// Pitch (Y축 회전)
	double sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		angles.y = std::copysign(XM_PI / 2, sinp); // 짐벌락 처리
	else
		angles.y = std::asin(sinp);

	// Yaw (Z축 회전)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	// 라디안 -> 도 변환
	angles.x = XMConvertToDegrees(angles.x);
	angles.y = XMConvertToDegrees(angles.y);
	angles.z = XMConvertToDegrees(angles.z);

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
		// X-Y-Z 순서로 회전 행렬 생성
		XMMATRIX rotX = XMMatrixRotationX(XMConvertToRadians(_Rotation.x));
		XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(_Rotation.y));
		XMMATRIX rotZ = XMMatrixRotationZ(XMConvertToRadians(_Rotation.z));

		XMMATRIX worldRotMatrix = rotX * rotY * rotZ;
		XMVECTOR worldQuatVec = XMQuaternionRotationMatrix(worldRotMatrix);

		// 부모 회전
		Vec3 parentRot = m_parent->GetRotation();
		XMMATRIX parentRotX = XMMatrixRotationX(XMConvertToRadians(parentRot.x));
		XMMATRIX parentRotY = XMMatrixRotationY(XMConvertToRadians(parentRot.y));
		XMMATRIX parentRotZ = XMMatrixRotationZ(XMConvertToRadians(parentRot.z));

		XMMATRIX parentRotMatrix = parentRotX * parentRotY * parentRotZ;
		XMVECTOR parentQuatVec = XMQuaternionRotationMatrix(parentRotMatrix);

		XMVECTOR parentInverseVec = XMQuaternionInverse(parentQuatVec);
		XMVECTOR localQuatVec = XMQuaternionMultiply(worldQuatVec, parentInverseVec);

		Quaternion localQuat;
		XMStoreFloat4(&localQuat, localQuatVec);

		Vec3 localRotation = ToEulerAngles(localQuat);
		SetLocalRotation(localRotation);
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


		Vec3 position = Vec3::Transform(_Position, worldToParentLocalMatrix);
		SetLocalPosition(position);
	}
	else {
		SetLocalPosition(_Position);
	}
}


