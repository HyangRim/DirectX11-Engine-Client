#pragma once
#include "ConstantBuffer.h"

struct GlobalDesc {
	Matrix V = Matrix::Identity;
	Matrix P = Matrix::Identity;
	Matrix VP = Matrix::Identity;
	Matrix Vinv = Matrix::Identity;
};

struct TransformDesc {
	Matrix W = Matrix::Identity;
};

// Light
struct LightDesc
{
	Color ambient = Color(1.f, 1.f, 1.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(1.f, 1.f, 1.f, 1.f);
	Color emissive = Color(1.f, 1.f, 1.f, 1.f);

	Vec3 direction;
	float padding0;
};

struct MaterialDesc
{
	Color ambient = Color(0.f, 0.f, 0.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(0.f, 0.f, 0.f, 1.f);
	Color emissive = Color(0.f, 0.f, 0.f, 1.f);
};

//Bone
#define MAX_BONE_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

struct BoneDesc {
	Matrix transforms[MAX_BONE_TRANSFORMS];
};

//Animation
struct KeyframeDesc {
	int32 m_animIndex = 0;
	uint32 m_currFrame = 0;

	//TODO
	uint32 m_nextFrame = 0;
	float m_ratio = 0.f;
	float m_sumTime = 0.f;
	float m_speed = 1.f;
	Vec2 padding;
};

struct TweenDesc {
	TweenDesc() {
		m_curr.m_animIndex = 0;
		m_next.m_animIndex = -1;
	}

	void ClearNextAnim() {
		m_next.m_animIndex = -1;
		m_next.m_currFrame = 0;
		m_next.m_nextFrame = 0;
		m_next.m_sumTime = 0;
		m_tweenSumTime = 0;
		m_tweenRatio = 0;
	}

	float m_tweenDuration = 1.0f;
	float m_tweenRatio = 0.f;
	float m_tweenSumTime = 0.f;
	float padding = 0.f;
	KeyframeDesc m_curr;
	KeyframeDesc m_next;
};

struct InstancedTweenDesc {
	TweenDesc tweens[MAX_MODEL_INSTANCE];
};

struct SnowBillboardDesc {
	Color color = Color(1, 1, 1, 1);

	Vec3 velocity = Vec3(0, -5, 0);
	float drawDistance = 0;

	Vec3 origin = Vec3(0, 0, 0);
	float turbulence = 5;

	Vec3 extent = Vec3(0, 0, 0);
	float time;
};