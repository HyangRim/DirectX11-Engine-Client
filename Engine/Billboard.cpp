#include "pch.h"
#include "Billboard.h"
#include "Material.h"
#include "Camera.h"
#include "Renderer.h"

Billboard::Billboard() : Super(ComponentType::Billboard)
{
	int32 vertexCount = MAX_BILLBOARD_COUNT * 4;
	int32 indexCount = MAX_BILLBOARD_COUNT * 6;

	m_vertices.resize(vertexCount);
	m_vertexBuffer = make_shared<VertexBuffer>();
	m_vertexBuffer->Create(m_vertices, 0, true);

	m_indices.resize(indexCount);

	for (int32 i = 0; i < MAX_BILLBOARD_COUNT; i++)
	{
		m_indices[i * 6 + 0] = i * 4 + 0;
		m_indices[i * 6 + 1] = i * 4 + 1;
		m_indices[i * 6 + 2] = i * 4 + 2;
		m_indices[i * 6 + 3] = i * 4 + 2;
		m_indices[i * 6 + 4] = i * 4 + 1;
		m_indices[i * 6 + 5] = i * 4 + 3;
	}

	m_indexBuffer = make_shared<IndexBuffer>();
	m_indexBuffer->Create(m_indices);

}

Billboard::~Billboard()
{
}

void Billboard::InnerRender(bool _isShadowTech)
{
	Super::InnerRender(_isShadowTech);


	//������ �ٲ���� �� ���ִ� ��. 
	if (m_drawCount != m_prevCount)
	{
		m_prevCount = m_drawCount;

		D3D11_MAPPED_SUBRESOURCE subResource;
		DC->Map(m_vertexBuffer->GetComPtr().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, m_vertices.data(), sizeof(VertexBillboard) * m_vertices.size());
		}
		DC->Unmap(m_vertexBuffer->GetComPtr().Get(), 0);
	}

	auto shader = m_material->GetShader();

	// IA
	m_vertexBuffer->PushData();
	m_indexBuffer->PushData();

	shader->DrawIndexed(GET_TECH(_isShadowTech), m_pass, m_drawCount * 6);

}

void Billboard::Add(Vec3 _pos, Vec2 _scale)
{
	m_vertices[m_drawCount * 4 + 0].pos = _pos;
	m_vertices[m_drawCount * 4 + 1].pos = _pos;
	m_vertices[m_drawCount * 4 + 2].pos = _pos;
	m_vertices[m_drawCount * 4 + 3].pos = _pos;
			   
	m_vertices[m_drawCount * 4 + 0].uv = Vec2(0, 1);
	m_vertices[m_drawCount * 4 + 1].uv = Vec2(0, 0);
	m_vertices[m_drawCount * 4 + 2].uv = Vec2(1, 1);
	m_vertices[m_drawCount * 4 + 3].uv = Vec2(1, 0);
			   
	m_vertices[m_drawCount * 4 + 0].scale = _scale;
	m_vertices[m_drawCount * 4 + 1].scale = _scale;
	m_vertices[m_drawCount * 4 + 2].scale = _scale;
	m_vertices[m_drawCount * 4 + 3].scale = _scale;

	++m_drawCount;
}

