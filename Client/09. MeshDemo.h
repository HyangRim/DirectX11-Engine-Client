#pragma once
#include "IExecute.h"
#include "Geometry.h"

class GameObject;

class MeshDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;


	shared_ptr<Shader> m_shader;


	//Object
	shared_ptr<GameObject> m_obj;

	//Camera
	shared_ptr<GameObject> m_camera;

	Vec3 m_lightDir = Vec3(0.f, 1.f, 0.f);
};

