#include "pch.h"
#include "02. QuadDemo.h"
#include "GeometryHelper.h"

void QuadDemo::Init()
{
	m_shader = make_shared<Shader>(L"02. Quad.fx");
	m_geometry = make_shared<Geometry<VertexColorData>>();
	GeometryHelper::CreateQuad(m_geometry, Color(1.f, 0.f, 0.f, 1.f));


	m_vertexBuffer = make_shared<VertexBuffer>();
	m_vertexBuffer->Create(m_geometry->GetVertices());

	m_indexBuffer = make_shared<IndexBuffer>();
	m_indexBuffer->Create(m_geometry->GetIndices());
}

void QuadDemo::Update()
{

}

void QuadDemo::Render()
{
	uint32 stride = m_vertexBuffer->GetStride();
	uint32 offset = m_vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, m_vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);

	DC->IASetIndexBuffer(m_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
	//_buffer->GetCount()
	//m_shader->Draw(0, 1, 3);
	m_shader->DrawIndexed(0, 0, m_indexBuffer->GetCount(), 0, 0);
}
