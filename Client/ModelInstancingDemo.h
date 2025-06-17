#pragma once
#include "IExecute.h"

class ModelInstancingDemo : public IExecute
{

public:
	void Init() override;
	void Update() override;
	void Render() override;


private:
	shared_ptr<Shader> m_shader;
	shared_ptr<GameObject> m_camera;
	vector<shared_ptr<GameObject>> m_objs;

};

