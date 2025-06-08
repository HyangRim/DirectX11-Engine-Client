#pragma once
#include "IExecute.h"
#include "Geometry.h"

class GameObject;

class NormalDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;


	shared_ptr<Shader> m_shader;


	//Object
	shared_ptr<Geometry<VertexTextureNormalData>> m_geometry;
	shared_ptr<VertexBuffer> m_vertexBuffer;
	shared_ptr<IndexBuffer> m_indexBuffer;
	Matrix m_world = Matrix::Identity;

	//Camera
	shared_ptr<GameObject> m_camera;
	shared_ptr<Texture> m_texture;

	Vec3 m_lightDir = Vec3(0.f, 1.f, 0.f);
};

