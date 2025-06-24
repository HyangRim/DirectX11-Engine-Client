#include "pch.h"
#include "BillboardDemo.h"
#include "GameObject.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "CameraScript.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Light.h"
#include "TextureBuffer.h"
#include "Viewport.h"
#include "SphereCollider.h"
#include "Scene.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"
#include "Terrain.h"
#include "Button.h"
#include "Billboard.h"
#include "SnowBillboard.h"
#include "ParticleSystem.h"
#include "Sky.h"

void BillboardDemo::Init()
{
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_shader = make_shared<Shader>(L"28. BillboardDemo.fx");

	// Camera
	{
		auto camera = make_shared<GameObject>();
		camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 3.f, -15.f });
		camera->AddComponent(make_shared<Camera>());
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
		CURSCENE->Add(camera);
	}

	// Light
	{
		auto light = make_shared<GameObject>();
		light->AddComponent(make_shared<Light>());
		LightDesc lightDesc;
		lightDesc.ambient = Vec4(0.4f);
		lightDesc.diffuse = Vec4(1.f);
		lightDesc.specular = Vec4(0.1f);
		lightDesc.direction = Vec3(1.f, 0.f, 1.f);
		light->GetLight()->SetLightDesc(lightDesc);
		CURSCENE->Add(light);
	}


	// Billboard
	{
		auto obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->SetLocalPosition(Vec3(0.f));
		obj->AddComponent(make_shared<Billboard>());
		{
			// Material
			{
				
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(m_shader);
				auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\grass.png");
				//auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
				material->SetDiffuseMap(texture);
				MaterialDesc& desc = material->GetMaterialDesc();
				desc.ambient = Vec4(1.f);
				desc.diffuse = Vec4(1.f);
				desc.specular = Vec4(1.f);
				RESOURCES->Add(L"Veigar", material);

				obj->GetBillboard()->SetMaterial(material);
			}
		}

		for (int32 i = 0; i < 500; i++)
		{

			Vec2 scale = Vec2(1 + rand() % 3, 1 + rand() % 3);
			Vec2 position = Vec2(-100 + rand() % 200, -100 + rand() % 200);

			obj->GetBillboard()->Add(Vec3(position.x, scale.y * 0.5f, position.y), scale);
		}

		CURSCENE->Add(obj);
	}
	// Terrain
	{
		auto terrainShader = make_shared<Shader>(L"23. RenderDemo.fx");
		{
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(terrainShader);
			auto texture = RESOURCES->Load<Texture>(L"TerrainGrass", L"..\\Resources\\Textures\\Terrain\\grass.jpg");
			material->SetDiffuseMap(texture);
			MaterialDesc& desc = material->GetMaterialDesc();
			desc.ambient = Vec4(1.f);
			desc.diffuse = Vec4(1.f);
			desc.specular = Vec4(1.f);
			RESOURCES->Add(L"TerrainGrass", material);
		}
		auto obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->SetLocalPosition(Vec3(-100.f, 0.f, -100.f));
		obj->AddComponent(make_shared<Terrain>());
		obj->GetTerrain()->Create(200, 200, RESOURCES->Get<Material>(L"TerrainGrass"));
		CURSCENE->Add(obj);
	}
	//Particle System

	{
		auto particleShader = make_shared<Shader>(L"ParticleSystem.fx");
		auto obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->SetLocalPosition(Vec3(0.f, 5.f, 0.f));
		obj->AddComponent(make_shared<ParticleSystem>());
		shared_ptr<ParticleSystem> particleSystem = obj->GetFixedComponent<ParticleSystem>(ComponentType::ParticleSystem);
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(particleShader);
		//auto texture = RESOURCES->Load<Texture>(L"TerrainGrass", L"..\\Resources\\Textures\\Terrain\\grass.jpg");
		material->SetDiffuseMap(RESOURCES->Get<Texture>(L"TerrainGrass"));
		material->SetRandomTex(RESOURCES->Get<Texture>(L"RandomTex"));
		particleSystem->SetMaterial(material);
		CURSCENE->Add(obj);
	}

	// Billboard
	{
		auto snowShader = make_shared<Shader>(L"29. SnowBillboard.fx");
		auto obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->SetLocalPosition(Vec3(0.f));
		obj->AddComponent(make_shared<SnowBillboard>(Vec3(100, 100, 100), 10000));
		{
			// Material
			{
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(snowShader);
				auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\grass.png");
				//auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
				material->SetDiffuseMap(texture);
				MaterialDesc& desc = material->GetMaterialDesc();
				desc.ambient = Vec4(1.f);
				desc.diffuse = Vec4(1.f);
				desc.specular = Vec4(1.f);
				RESOURCES->Add(L"Veigar", material);

				obj->GetSnowBillboard()->SetMaterial(material);
			}
		}

		CURSCENE->Add(obj);
	}


	// UI
	{
		auto obj = make_shared<GameObject>();
		obj->SetLayerIndex(LAYER_UI);
		obj->AddComponent(make_shared<Button>());
		obj->GetButton()->Create(Vec2(100, 100), Vec2(100, 100), RESOURCES->Get<Material>(L"Veigar"));
		obj->GetButton()->AddOnClikedEvent([obj]() { CURSCENE->Remove(obj); });

		CURSCENE->Add(obj);
	}

	{
		// UICamera
		auto camera = make_shared<GameObject>();
		camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
		camera->GetCamera()->SetNear(1.0f);
		camera->GetCamera()->SetFar(100.0f);
		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, false);
		CURSCENE->Add(camera);
	}

	//Sky
	{
		CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\snowcube1024.dds", L"Sky.fx"));
	}
}

void BillboardDemo::Update()
{
}

void BillboardDemo::Render()
{

}

//버튼이라는 클래스는 OBB Collision을 두고. 
//Ray를 쏘는 방식으로...
//그런데, 우리는 그냥 WINAPI방식으로 한다. Collision방식은 부하가 있음. 
//그림자는 무조건 들어가는 게 좋음. 

//포폴을 만들 때 중요한 건 시간과 노력
//충돌, 레이캐스팅, 애니메이션, 매쉬 로드등과 같은 것. 

void BillboardTest::Update()
{
	auto go = GetGameObject();
	Vec3 cameraPos = CURSCENE->GetMainCamera()->GetTransform()->GetPosition();

	Vec3 myPos = GetTransform()->GetPosition();
	Vec3 up = Vec3(0, 1, 0);
	Vec3 forward = cameraPos - myPos;
	forward.Normalize();

	Matrix lookMatrix = Matrix::CreateWorld(myPos, forward, up);

	Vec3 S, T;
	Quaternion R;
	lookMatrix.Decompose(S, R, T);

	Vec3 rot = Transform::ToEulerAngles(R);

	GetTransform()->SetRotation(rot);

	//myPos -> cameraPos 는 그냥 방향 벡터 구해줘서 normalize해주면 됨

	//right는 그 방향 벡터에 UpVector를 외적하면 right벡터가 구해짐. 
}
