#pragma once
#include "MonoBehaviour.h"
class CameraScript : public MonoBehaviour
{
public:

private:
	virtual void Init() override;
	virtual void Update() override;


public:
	void SetFollowTarget(shared_ptr<GameObject> _target) {
		m_FollowingTarget = _target;
	}

private:
	void characterFollow();

private:
	shared_ptr<GameObject> m_FollowingTarget;

	float m_speed = 10.f;
	bool isFollowing = false;
};

