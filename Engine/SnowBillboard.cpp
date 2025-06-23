#include "pch.h"
#include "SnowBillboard.h"
#include "Material.h"
#include "Camera.h"
#include "MathUtils.h"


SnowBillboard::SnowBillboard(Vec3 _extent, int32 _drawCount /*= 100*/)
	:Super(ComponentType::SnowBillboard)
{
	m_desc.extent = _extent;
	m_desc.drawDistance = _extent.z * 2.0f;
	m_drawCount = _drawCount;

	const int32 vertexCount = _drawCount * 4;
	m_vertices.resize(vertexCount);

	for (int32 i = 0; i < m_drawCount * 4; i += 4)
	{
		Vec2 scale = MathUtils::RandomVec2(0.1f, 0.5f);

		Vec3 position;
		position.x = MathUtils::Random(-m_desc.extent.x, m_desc.extent.x);
		position.y = MathUtils::Random(-m_desc.extent.y, m_desc.extent.y);
		position.z = MathUtils::Random(-m_desc.extent.z, m_desc.extent.z);

		Vec2 random = MathUtils::RandomVec2(0.0f, 1.0f);

		m_vertices[i + 0].pos = position;
		m_vertices[i + 1].pos = position;
		m_vertices[i + 2].pos = position;
		m_vertices[i + 3].pos = position;
		
		m_vertices[i + 0].uv = Vec2(0, 1);
		m_vertices[i + 1].uv = Vec2(0, 0);
		m_vertices[i + 2].uv = Vec2(1, 1);
		m_vertices[i + 3].uv = Vec2(1, 0);
		
		m_vertices[i + 0].scale = scale;
		m_vertices[i + 1].scale = scale;
		m_vertices[i + 2].scale = scale;
		m_vertices[i + 3].scale = scale;
		
		m_vertices[i + 0].random = random;
		m_vertices[i + 1].random = random;
		m_vertices[i + 2].random = random;
		m_vertices[i + 3].random = random;
	}

	m_vertexBuffer = make_shared<VertexBuffer>();
	m_vertexBuffer->Create(m_vertices, 0);

	const int32 indexCount = m_drawCount * 6;
	m_indices.resize(indexCount);

	for (int32 i = 0; i < m_drawCount; i++)
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

SnowBillboard::~SnowBillboard()
{

}

void SnowBillboard::Update()
{
	m_desc.origin = CURSCENE->GetMainCamera()->GetTransform()->GetPosition();
	m_desc.time = m_elapsedTime;
	m_elapsedTime += DT;

	auto shader = m_material->GetShader();

	// Transform
	auto world = GetTransform()->GetWorldMatrix();
	shader->PushTransformData(TransformDesc{ world });

	// GlobalData
	shader->PushGlobalData(Camera::s_MatView, Camera::s_MatProjection);

	// SnowData
	shader->PushSnowData(m_desc);

	// Light
	m_material->Update();

	// IA
	m_vertexBuffer->PushData();
	m_indexBuffer->PushData();

	shader->DrawIndexed(0, m_pass, m_drawCount * 6);
}
