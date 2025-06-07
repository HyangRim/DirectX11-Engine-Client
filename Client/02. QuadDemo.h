#pragma once
#include "IExecute.h"
#include "Geometry.h"
class QuadDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;


	shared_ptr<Shader> m_shader;

	shared_ptr<Geometry<VertexColorData>> m_geometry;
	shared_ptr<VertexBuffer> m_vertexBuffer;
	shared_ptr<IndexBuffer> m_indexBuffer;
};

