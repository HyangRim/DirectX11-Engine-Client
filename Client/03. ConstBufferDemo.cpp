#include "pch.h"
#include "03. ConstBufferDemo.h"
#include "GeometryHelper.h"

void ConstBufferDemo::Init()
{
	m_shader = make_shared<Shader>(L"03. ConstBuffer.fx");
	m_geometry = make_shared<Geometry<VertexColorData>>();
	GeometryHelper::CreateQuad(m_geometry, Color(1.f, 0.f, 0.f, 1.f));


	m_vertexBuffer = make_shared<VertexBuffer>();
	m_vertexBuffer->Create(m_geometry->GetVertices());

	m_indexBuffer = make_shared<IndexBuffer>();
	m_indexBuffer->Create(m_geometry->GetIndices());
}

void ConstBufferDemo::Update()
{
	float dt = TIME->GetDeltaTime();

	if (INPUT->GetButton(KEY_TYPE::A)) {
		m_translation.x -= 2.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::D)) {
		m_translation.x += 2.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::W)) {
		m_translation.y += 2.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::S)) {
		m_translation.y -= 2.f * dt;
	}

	//SRT °öÇÏ±â. 
	m_world = Matrix::CreateTranslation(m_translation);
}

void ConstBufferDemo::Render()
{
	m_shader->GetMatrix("World")->SetMatrix((float*)&m_world);
	m_shader->GetMatrix("View")->SetMatrix((float*)&m_view);
	m_shader->GetMatrix("Projection")->SetMatrix((float*)&m_projection);

	uint32 stride = m_vertexBuffer->GetStride();
	uint32 offset = m_vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, m_vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);

	DC->IASetIndexBuffer(m_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
	//_buffer->GetCount()
	//m_shader->Draw(0, 1, 3);
	m_shader->DrawIndexed(0, 0, m_indexBuffer->GetCount(), 0, 0);
}
