#pragma once

#include "Primitive3D.h"
struct MathUtils
{

	//���� ������ �� ������ �뷫������ ������ �� �־�� �Ѵ�. 
	//��� AABB, OBB, Plane�� �� ���� ���ԵǾ� �ִ��� üũ�ϱ� ����. 
	//Point Test

	// SphereToPoint
	static bool PointInSphere(const Point3D& _point, const Sphere3D& _sphere);
	static Point3D ClosestPoint(const Sphere3D& _sphere, const Point3D& _point);

	//AABBToPoint
	static bool PointInAABB(const Point3D& _point, const AABB3D& _aabb);
	static Point3D ClosestPoint(const AABB3D& _aabb, const Point3D& _point);

	//OBBToPoint;
	static bool PointInOBB(const Point3D& _point, const OBB3D& _obb);
	static Point3D ClosestPoint(const OBB3D& _obb, const Point3D& _point);

	//Plane
	//� ���� Plane�� �ִ��� �׽�Ʈ �ϰ� ����. 
	//����� ������� 
	//� ���� ��Ȯ�� ��� ���� �ִ��� Ȯ�� �ϱ� ���ؼ�, � ���������, ��� �߾������� ���͸� ���ϰ�.
	//����� �븻���Ͷ� ������ �ϸ�, ���󰡴� ���� ���� �Ÿ� ���Ͱ� ����. 
	//PlaneToPoint;
	static bool PointInPlane(const Point3D& _point, const Plane3D& _plane);
	static Point3D ClosestPoint(const Plane3D& _plane, const Point3D& _point);

	//� ������ �ִµ� C��� Point�� �� �ȿ� ���ԵǾ� �ִ°�?
	//LineToPoint
	static bool PointInLine(const Point3D& _point, const Line3D& _line);
	static Point3D ClosestPoint(const Line3D& _line, const Point3D& _point);


	//PointAndRay
	//Ray�� ������ �ְ� ������ �����Ŵ�. 
	//������ �ְ�, P��� ���� ���� ����� ���� ���ϰ� ���� ��. 
	//     R
	//     I
	//O -------> R
	static bool PointInRay(const Point3D& _point, const Ray3D& _ray);
	static Point3D ClosestPoint(const Ray3D& _ray, const Point3D& _point);





	//////////////////////////////////////////////////////////
	//Intersections

	static bool SphereSphere(const Sphere3D& _s1, const Sphere3D& _s2);
	static bool SphereAABB(const Sphere3D& _sphere, const AABB3D& _aabb);
	static bool SphereOBB(const Sphere3D& _sphere, const OBB3D& _obb);
	static bool SpherePlane(const Sphere3D& _sphere, const Plane3D& _plane);
	static bool AABBAABB(const AABB3D& _aabb1, const AABB3D& _aabb2);


	//���� ���� ������� �׽�Ʈ ������Ѵ�. 
	//� ���� ������� �����ؼ� ��ġ�� ����� ������, ���� ������� �����. 
	//������ ���߿� ������� �ִٸ�, return false;
	//SAT���. 
	static Interval3D GetInterval(const AABB3D& _aabb, const Vec3& _axis);
	static Interval3D GetInterval(const OBB3D& _obb, const Vec3& _axis);
	static bool OverlapOnAxis(const AABB3D& _aabb, const OBB3D& _obb, const Vec3& _axis);
	static bool OverlapOnAxis(const OBB3D& _obb1, const OBB3D& _obb2, const Vec3& _axis);
	static bool AABBOBB(const AABB3D& _aabb, const OBB3D& _obb);
	
	//����� ���Ѵ�� �� �¾����. 
	//��� �������� ������� ���� �ִ��� �Ʒ��� �ִ��� �Ǻ� 
	//������ ����ϸ� �ȴ�. 
	static bool AABBPlane(const AABB3D& _aabb, const Plane3D& _plane);
	static bool OBBOBB(const OBB3D& _obb1, const OBB3D& _obb2);
	static bool PlanePlane(const Plane3D& _plane1, const Plane3D& _plane2);

	//////////////////////////////////////////////////////////
	//Raycast

	static bool RayCast(const Sphere3D& _sphere, const Ray3D& _ray, OUT float& _distance);
	static bool RayCast(const AABB3D& _aabb, const Ray3D& _ray, OUT float& _distance);
	static bool RayCast(const Plane3D& _plane, const Ray3D& _ray, OUT float& _distance);

	//�п� ���� NAV Mesh. �� �� �ִ� ������ �����ϰ�, �װ� ����� �ְ� �ͳ�. 
	//�װ� ���� ���� �ϴ� ����� �ﰢ�� ���� �� �׷��ִ� ���. 

	//�⺻������ ������ ����Ѵ�. ���� ���͸� ���Ѵ����� CROSS�� POINT IN TRIANGLE
	static bool PointInTriangle(const Point3D& _p, const Triangle3D& _t);

	static Plane3D FromTriangle(const Triangle3D& _t);
	static Vec3 Barycentric(const Point3D& _P, const Triangle3D& _t);
	static bool RayCast(const Triangle3D& _triangle, const Ray3D& _ray, OUT float& _distance);
	static Vec3 ProjectVecOnVec(Vec3 _from, Vec3 _to);
};

