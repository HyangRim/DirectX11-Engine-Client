#include "pch.h"
#include "GeometryHelper.h"

void GeometryHelper::CreateQuad(shared_ptr<Geometry<VertexColorData>> _geometry, Color _color)
{
	vector<VertexColorData> vertex;
	vertex.resize(4);

	vertex[0].position = Vec3(-0.5f, -0.5f, 0.f);
	vertex[0].color = _color;

	vertex[1].position = Vec3(-0.5f, 0.5f, 0.f);
	vertex[1].color = _color;

	vertex[2].position = Vec3(0.5f, -0.5f, 0.f);
	vertex[2].color = _color;

	vertex[3].position = Vec3(0.5f, 0.5f, 0.f);
	vertex[3].color = _color;

	_geometry->SetVertices(vertex);

	vector<uint32> index = { 0,1,2,2,1,3 };
	_geometry->SetIndices(index);
}
