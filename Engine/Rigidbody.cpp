#include "pch.h"
#include "GameObject.h"
#include "Rigidbody.h"
#include "AABBBoxCollider.h"
#include "SphereCollider.h"
#include "OBBBoxCollider.h"
#include "MathUtils.h"
#include <cmath>

Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
	, m_mass(1.f), m_fricCoeff(3.f), m_maxVelocity(Vec3(5.f, 5.f, 5.f))
{
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::FixedUpdate()
{
	float scala = m_Force.Length(); //���� ũ��. 

	//���� ����ȭ ���ְ�. 
	m_Force.Normalize();

	float accel = scala / m_mass; //�������� ���ӵ�.
	m_Accel = m_Force * accel;	//���� * ��Į�� / ����

	//m_Accel += m_vAccel;
	m_velocity += m_Accel * DT;

	//���ӵ� ���� �ڵ�. 
	Vec3 fricDir = -m_velocity;
	if (fricDir.Length() > 0.001f) {
		fricDir.Normalize();
	}

	//������� ������ �ݴ� ���� ����. 
	Vec3 friction = fricDir * m_fricCoeff * DT;

	if (m_velocity.Length() <= friction.Length()) {
		m_velocity = Vec3(0.f, 0.f, 0.f);
	}
	else {
		m_velocity += friction;
	}

	//�ӵ� ���� �˻�. 
	//�� ���� ���� �˻��ؾ���.

	Move();

	//�� �ʱ�ȭ
	m_Force = Vec3(0, 0, 0);
	m_Accel = Vec3(0, 0, 0);
}

void Rigidbody::OnCollision(shared_ptr<GameObject> _other)
{
	if (_other == nullptr)
		return;

	shared_ptr<BaseCollider> myCollider = GetGameObject()->GetCollider();
	shared_ptr<BaseCollider> otherCollider = _other->GetCollider();

	if (myCollider == nullptr || otherCollider == nullptr) 
		return;

	Vec3 dir;
	float depth;

	if (ComputePushMove(myCollider, otherCollider, dir, depth)) {
		Vec3 correction = dir * depth;

		Vec3 myPos = GetTransform()->GetPosition();
		Vec3 otherPos = _other->GetTransform()->GetPosition();

		myPos += correction * 0.5f;
		otherPos -= correction * 0.5f;

		GetTransform()->SetPosition(myPos);
		_other->GetTransform()->SetPosition(otherPos);

		m_velocity = Vec3(0, 0, 0);
		_other->GetRigidbody()->SetVelocity(Vec3(0, 0, 0));
	}
}

void Rigidbody::OnCollisionEnter(shared_ptr<GameObject> _other)
{
}

void Rigidbody::OnCollisionExit(shared_ptr<GameObject> _other)
{
}

bool Rigidbody::ComputePushMove(shared_ptr<BaseCollider> _my, shared_ptr<BaseCollider> _other, Vec3& _outDir, float& _outForce)
{
	//Sphere - Sphere 
	{
		auto SphereA = dynamic_pointer_cast<SphereCollider>(_my);
		auto SphereB = dynamic_pointer_cast<SphereCollider>(_other);

		
		if (SphereA && SphereB) {
			auto& boundA = SphereA->GetBoundSphere();
			auto& boundB = SphereB->GetBoundSphere();

			Vec3 centerA = Vec3(boundA.Center.x, boundA.Center.y, boundA.Center.z);
			Vec3 centerB = Vec3(boundB.Center.x, boundB.Center.y, boundB.Center.z);

			Vec3 dir = centerB - centerA;
			Vec3 dirNormalized = dir;
			dirNormalized.Normalize();
			float dist = dir.Length();
			float radius = boundA.Radius + boundB.Radius;

			_outDir = dist > 0.001f ? dirNormalized : Vec3(0, 0, 0);
			_outForce = radius - dist;
			return true;
		}
	}


	// AABB - Sphere, Sphere - AABB
	{
		auto AABBA = dynamic_pointer_cast<AABBBoxCollider>(_my);
		auto SphereB = dynamic_pointer_cast<SphereCollider>(_other);

		if (AABBA && SphereB) {
			auto& box = AABBA->GetBoundingBox();
			auto& sphere = SphereB->GetBoundSphere();

			Vec3 closest = box.Center;

			closest.x = XMMax(box.Center.x - box.Extents.x, std::min(sphere.Center.x, box.Center.x + box.Extents.x));
			closest.y = XMMax(box.Center.y - box.Extents.y, std::min(sphere.Center.y, box.Center.y + box.Extents.y));
			closest.z = XMMax(box.Center.z - box.Extents.z, std::min(sphere.Center.z, box.Center.z + box.Extents.z));

			Vec3 dir = sphere.Center - closest;
			float distSq = dir.LengthSquared();

			if (distSq > sphere.Radius * sphere.Radius)
				return false;

			float dist = sqrtf(distSq);
			dir.Normalize();
			_outDir = dist > 0.0001f ? dir : Vec3(0, 0, 0);
			_outForce = sphere.Radius - dist;
			return true;
		}

		auto SphereA = dynamic_pointer_cast<SphereCollider>(_my);
		auto AABBB = dynamic_pointer_cast<AABBBoxCollider>(_other);
		if (SphereA && AABBB) {
			return ComputePushMove(_other, _my, _outDir, _outForce);
		}
	}

	//OBB - Sphere , Sphere - OBB
	{
		auto obba = dynamic_pointer_cast<OBBBoxCollider>(_my);
		auto sphb = dynamic_pointer_cast<SphereCollider>(_other);

		if (obba && sphb) {
			auto& box = obba->GetBoundingBox();
			auto& sphere = sphb->GetBoundSphere();

			Vec3 closest;
			
			//TODO : closest ����ϱ�. 
			XMVECTOR boxCenter = XMLoadFloat3(&box.Center);
			XMVECTOR sphereCenter = XMLoadFloat3(&sphere.Center);
			XMVECTOR toSphere = XMVectorSubtract(sphereCenter, boxCenter);

			// OBB�� ȸ�� ��� ����ȯ
			XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&box.Orientation));
			XMMATRIX invRotMatrix = XMMatrixTranspose(rotMatrix);

			XMVECTOR localSphere = XMVector3Transform(toSphere, invRotMatrix);
			XMFLOAT3 local;
			XMStoreFloat3(&local, localSphere);

			// AABB-Sphereó�� �� �࿡�� Ŭ���� (���� ��ǥ�迡��)
			local.x = XMMax(-box.Extents.x, std::min(box.Extents.x, local.x));
			local.y = XMMax(-box.Extents.y, std::min(box.Extents.y, local.y));
			local.z = XMMax(-box.Extents.z, std::min(box.Extents.z, local.z));

			XMVECTOR localClosest = XMLoadFloat3(&local);
			XMVECTOR worldClosest = XMVector3Transform(localClosest, rotMatrix);
			XMVECTOR finalClosest = XMVectorAdd(boxCenter, worldClosest);

			XMFLOAT3 closestFloat3;
			XMStoreFloat3(&closestFloat3, finalClosest);
			closest = Vec3(closestFloat3.x, closestFloat3.y, closestFloat3.z);

			Vec3 dir = sphere.Center - closest;
			float distSq = dir.LengthSquared();
			if (distSq > sphere.Radius * sphere.Radius)
				return false;

			float dist = sqrtf(distSq);
			dir.Normalize();
			_outDir = dist > 0.0001f ? dir : Vec3(0, 0, 0);
			_outForce = sphere.Radius - dist;
			return true;
		}

		auto spha = dynamic_pointer_cast<SphereCollider>(_my);
		auto obbb = dynamic_pointer_cast<OBBBoxCollider>(_other);

		if (spha && obbb)
			return ComputePushMove(_other, _my, _outDir, _outForce);
	}

	//AABB - AABB
	{
		auto aabba = dynamic_pointer_cast<AABBBoxCollider>(_my);
		auto aabbb = dynamic_pointer_cast<AABBBoxCollider>(_other);

		if (aabba && aabbb) {
			auto& boxA = aabba->GetBoundingBox();
			auto& boxB = aabbb->GetBoundingBox();

			if (!boxA.Intersects(boxB))
				return false;

			Vec3 centerA = Vec3(boxA.Center.x, boxA.Center.y, boxA.Center.z);
			Vec3 centerB = Vec3(boxB.Center.x, boxB.Center.y, boxB.Center.z);
			Vec3 dir = centerB - centerA;

			Vec3 extentsA = Vec3(boxA.Extents.x, boxA.Extents.y, boxA.Extents.z);
			Vec3 extentsB = Vec3(boxB.Extents.x, boxB.Extents.y, boxB.Extents.z);

			// �� �࿡���� ��ø �Ÿ� ���
			float overlapX = (extentsA.x + extentsB.x) - abs(dir.x);
			float overlapY = (extentsA.y + extentsB.y) - abs(dir.y);
			float overlapZ = (extentsA.z + extentsB.z) - abs(dir.z);

			// �浹���� ���� ���
			if (overlapX <= 0 || overlapY <= 0 || overlapZ <= 0)
				return false;

			// ���� ���� ��ø ���� �и� ������ ����
			if (overlapX < overlapY && overlapX < overlapZ) {
				_outDir = Vec3(dir.x > 0 ? 1.0f : -1.0f, 0, 0);
				_outForce = overlapX;
			}
			else if (overlapY < overlapZ) {
				_outDir = Vec3(0, dir.y > 0 ? 1.0f : -1.0f, 0);
				_outForce = overlapY;
			}
			else {
				_outDir = Vec3(0, 0, dir.z > 0 ? 1.0f : -1.0f);
				_outForce = overlapZ;
			}

			return true;
		}
	}


	//AABB - OBB
	{
		auto aabba = dynamic_pointer_cast<AABBBoxCollider>(_my);
		auto obbb = dynamic_pointer_cast<OBBBoxCollider>(_other);

		if (aabba && obbb) {
			auto& boxA = aabba->GetBoundingBox();
			auto& boxB = obbb->GetBoundingBox();

			if (!boxA.Intersects(boxB))
				return false;

			Vec3 centerA = Vec3(boxA.Center.x, boxA.Center.y, boxA.Center.z);
			Vec3 centerB = Vec3(boxB.Center.x, boxB.Center.y, boxB.Center.z);
			Vec3 dir = centerB - centerA;

			if (dir.LengthSquared() < 0.0001f)
				return false;
			else 
				dir.Normalize();

			XMFLOAT3 extA = boxA.Extents;
			XMFLOAT3 extB = boxB.Extents;
			
			float avgSizeA = (extA.x + extA.y + extA.z) / 3.0f;
			float avgSizeB = (extB.x + extB.y + extB.z) / 3.0f;
			_outForce = (avgSizeA + avgSizeB) * 0.1f;
			_outDir = dir;
			return true;
		}

		auto obba = dynamic_pointer_cast<OBBBoxCollider>(_my);
		auto aabbb = dynamic_pointer_cast<AABBBoxCollider>(_other);
		if (obba && aabbb) {
			return ComputePushMove(_other, _my, _outDir, _outForce);
		}
	}

	// OBB - OBB 
	{
		auto obba = dynamic_pointer_cast<OBBBoxCollider>(_my);
		auto obbb = dynamic_pointer_cast<OBBBoxCollider>(_other);

		if (obba && obbb) {
			auto& boxa = obba->GetBoundingBox();
			auto& boxb = obbb->GetBoundingBox();

			if (!boxa.Intersects(boxb))
				return false;

			//�ٻ� ó��. 
			Vec3 centerA = Vec3(boxa.Center.x, boxa.Center.y, boxa.Center.z);
			Vec3 centerB = Vec3(boxb.Center.x, boxb.Center.y, boxb.Center.z);
			Vec3 dir = centerB - centerA;
			if (dir.LengthSquared() < 0.0001f)
				return false;
			else
				dir.Normalize();

			XMFLOAT3 extA = boxa.Extents;
			XMFLOAT3 extB = boxb.Extents;

			float lenA = sqrtf(extA.x * extA.x + extA.y * extA.y + extA.z * extA.z);
			float lenB = sqrtf(extB.x * extB.x + extB.y * extB.y + extB.z * extB.z);

			_outDir = dir;
			_outForce = std::min(lenA, lenB) * 0.1f;
			return true;
		}
	}

	return false;
}

void Rigidbody::Move()
{
	float speed = m_velocity.Length();

	if (speed >= 0.001f) {
		Vec3 pos = GetTransform()->GetPosition();
		pos += m_velocity * DT;

		GetTransform()->SetPosition(pos);
	}
}
