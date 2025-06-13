#pragma once
#include "IExecute.h"

class InstancingDemo : public IExecute
{

public:
	void Init() override;
	void Update() override;
	void Render() override;


private:
	shared_ptr<Shader> m_shader;
	shared_ptr<GameObject> m_camera;
	vector<shared_ptr<GameObject>> m_objs;


private:
	//INSTANCING
	shared_ptr<Mesh> m_mesh;
	shared_ptr<Material> m_material;

	vector<Matrix> m_worlds;
	shared_ptr<VertexBuffer> m_instanceBuffer;
};

