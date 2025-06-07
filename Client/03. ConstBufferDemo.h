#pragma once
#include "IExecute.h"
#include "Geometry.h"
class ConstBufferDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;


	shared_ptr<Shader> m_shader;

	shared_ptr<Geometry<VertexColorData>> m_geometry;
	shared_ptr<VertexBuffer> m_vertexBuffer;
	shared_ptr<IndexBuffer> m_indexBuffer;

	Vec3 m_translation = Vec3(0.f, 0.f, 0.f);

	Matrix m_world = Matrix::Identity;
	Matrix m_view = Matrix::Identity;
	Matrix m_projection = Matrix::Identity;
};

