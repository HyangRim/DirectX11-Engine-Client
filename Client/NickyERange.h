#pragma once
#include "GameObject.h"

class NickyERange
	: public GameObject
{
public:
	NickyERange();
	~NickyERange();

public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnCollisionEnter(shared_ptr<GameObject> _other) override;

private:
	float m_timer = 0.f;
	float m_lifeTime = 1.5f;
};

