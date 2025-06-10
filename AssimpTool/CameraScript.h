#pragma once
#include "MonoBehaviour.h"
class CameraScript : public MonoBehaviour
{
public:

private:
	virtual void Init() override;
	virtual void Update() override;

	float m_speed = 10.f;
};

