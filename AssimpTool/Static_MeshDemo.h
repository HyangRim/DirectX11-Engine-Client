#pragma once
#include "IExecute.h"

class Static_MeshDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

	void CreateTower();
	void CreateTank();

private:
	shared_ptr<Shader> m_shader;
	shared_ptr<GameObject> m_obj;
	shared_ptr<GameObject> m_camera;
};

