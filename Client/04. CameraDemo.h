#pragma once
#include "IExecute.h"
#include "Geometry.h"

class GameObject;

class CameraDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;


	shared_ptr<Shader> m_shader;


	//Object
	shared_ptr<Geometry<VertexColorData>> m_geometry;
	shared_ptr<VertexBuffer> m_vertexBuffer;
	shared_ptr<IndexBuffer> m_indexBuffer;
	Matrix m_world = Matrix::Identity;

	//Camera
	shared_ptr<GameObject> m_camera;
};

