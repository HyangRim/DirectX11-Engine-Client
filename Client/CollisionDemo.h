#pragma once
#include "IExecute.h"

class CollisionDemo : public IExecute
{

public:
	void Init() override;
	void Update() override;
	void Render() override;

private:
	shared_ptr<Shader> m_shader;

};

#include "MonoBehaviour.h"

class MoveScript : public MonoBehaviour {
public:
	virtual void Update() override;
};

