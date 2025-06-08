#include "pch.h"
#include "05. TextureDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Texture.h"
#include "Transform.h"

void TextureDemo::Init()
{
	m_shader = make_shared<Shader>(L"04. Texture.fx");


	m_geometry = make_shared<Geometry<VertexTextureData>>();
	//GeometryHelper::CreateQuad(m_geometry);
	GeometryHelper::CreateSphere(m_geometry);
	GeometryHelper::CreateGrid(m_geometry, 256, 256);

	m_vertexBuffer = make_shared<VertexBuffer>();
	m_vertexBuffer->Create(m_geometry->GetVertices());

	m_indexBuffer = make_shared<IndexBuffer>();
	m_indexBuffer->Create(m_geometry->GetIndices());


	//Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetOrAddTransform();
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());
	m_camera->GetTransform()->SetPosition(Vec3(0.f, 0.f, -2.f));

	m_texture = RESOURCES->Load<Texture>(L"veigar", L"..\\Resources\\Textures\\veigar.jpg");
	
}

void TextureDemo::Update()
{
	m_camera->Update();

}

void TextureDemo::Render()
{
	m_shader->GetMatrix("World")->SetMatrix((float*)&m_world);
	m_shader->GetMatrix("View")->SetMatrix((float*)&Camera::s_MatView);
	m_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::s_MatProjection);
	m_shader->GetSRV("Texture0")->SetResource(m_texture->GetComPtr().Get());


	uint32 stride = m_vertexBuffer->GetStride();
	uint32 offset = m_vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, m_vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);

	DC->IASetIndexBuffer(m_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
	//_buffer->GetCount()
	//m_shader->Draw(0, 1, 3);
	m_shader->DrawIndexed(0, 0, m_indexBuffer->GetCount(), 0, 0);
}
