#include "pch.h"
#include "07. HeightDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Texture.h"
#include "Transform.h"

void HeightDemo::Init()
{
	m_shader = make_shared<Shader>(L"06. World.fx");

	//Texture
	m_heightMap = RESOURCES->Load<Texture>(L"Height", L"..\\Resources\\Textures\\Terrain\\height.png");
	m_texture = RESOURCES->Load<Texture>(L"veigar", L"..\\Resources\\Textures\\Terrain\\grass.jpg");

	const int32 width = m_heightMap->GetSize().x;
	const int32 height = m_heightMap->GetSize().y;

	const DirectX::ScratchImage& info = m_heightMap->GetInfo();
	uint8* pixelBuffer = info.GetPixels();


	m_geometry = make_shared<Geometry<VertexTextureData>>();
	//GeometryHelper::CreateQuad(m_geometry);
	//GeometryHelper::CreateSphere(m_geometry);
	GeometryHelper::CreateGrid(m_geometry, width, height);

	//CPU
	{
		//강제로 const를 떼는 casting
		vector<VertexTextureData>& v = const_cast<vector<VertexTextureData>&>(m_geometry->GetVertices());
		for (int32 z = 0; z < height; ++z) {
			for (int32 x = 0; x < height; ++x) {
				int32 idx = width * z + x;

				uint8 height = pixelBuffer[idx] / 255.f * 25.f;
				v[idx].position.y = height;
			}
		}
	}

	m_vertexBuffer = make_shared<VertexBuffer>();
	m_vertexBuffer->Create(m_geometry->GetVertices());

	m_indexBuffer = make_shared<IndexBuffer>();
	m_indexBuffer->Create(m_geometry->GetIndices());


	//Camera
	m_camera = make_shared<GameObject>();
	m_camera->GetOrAddTransform();
	m_camera->AddComponent(make_shared<Camera>());
	m_camera->AddComponent(make_shared<CameraScript>());
	m_camera->GetTransform()->SetPosition(Vec3(0.f, 5.f, 0.f));
	m_camera->GetTransform()->SetRotation(Vec3(25.f, 0.f, 0.f));
}

void HeightDemo::Update()
{
	m_camera->Update();

}

void HeightDemo::Render()
{
	m_shader->GetMatrix("World")->SetMatrix((float*)&m_world);
	m_shader->GetMatrix("View")->SetMatrix((float*)&Camera::s_MatView);
	m_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::s_MatProjection);
	m_shader->GetSRV("Texture0")->SetResource(m_texture->GetComPtr().Get());

	enum ADDRESS_VALUE {
		ADDRESS_WRAP = 0,
		ADDRESS_MIRROR = 1,
		ADDRESS_CLAMP = 2,
		ADDRESS_BORDER = 3,
	};
	m_shader->GetScalar("Address")->SetInt(ADDRESS_BORDER);

	uint32 stride = m_vertexBuffer->GetStride();
	uint32 offset = m_vertexBuffer->GetOffset();



	DC->IASetVertexBuffers(0, 1, m_vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);

	DC->IASetIndexBuffer(m_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
	//_buffer->GetCount()
	//m_shader->Draw(0, 1, 3);
	m_shader->DrawIndexed(0, 1, m_indexBuffer->GetCount(), 0, 0);
}
