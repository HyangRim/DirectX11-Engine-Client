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
	CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\snowcube1024.dds", L"Sky.fx"));
	shared_ptr<Shader> renderShader = make_shared<Shader>(L"23. RenderDemo.fx");
	{
		// Camera
		auto camera = make_shared<GameObject>();
		camera->GetTransform()->SetPosition(Vec3{ 0.f, 2.f, -15.f });
		camera->AddComponent(make_shared<Camera>());
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
		CURSCENE->Add(camera);
	}

	{
		// Mesh
		// Material
		{
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(renderShader);
			auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
			material->SetDiffuseMap(texture);
			MaterialDesc& desc = material->GetMaterialDesc();
			desc.ambient = Vec4(1.f);
			desc.diffuse = Vec4(1.f);
			desc.specular = Vec4(1.f);
			RESOURCES->Add(L"Veigar", material);
		}

		for (int32 i = 0; i < 1; i++)
		{
			auto obj = make_shared<GameObject>();
			obj->GetTransform()->SetLocalPosition(Vec3(0, 2.5f, 0));
			obj->GetTransform()->SetLocalScale(Vec3(1.f));
			obj->AddComponent(make_shared<MeshRenderer>());
			{
				obj->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"Veigar"));
			}
			{
				auto mesh = RESOURCES->Get<Mesh>(L"Cube");
				obj->GetMeshRenderer()->SetMesh(mesh);
				obj->GetMeshRenderer()->SetPass(0);
			}
			{
				auto collider = make_shared<OBBBoxCollider>();
				obj->AddComponent(collider);
			}

			CURSCENE->Add(obj);
		}
	}

	{
		// Light
		auto light = make_shared<GameObject>();
		light->AddComponent(make_shared<Light>());

		LightDesc lightDesc;
		lightDesc.ambient = Vec4(0.4f);
		lightDesc.diffuse = Vec4(1.f);
		lightDesc.specular = Vec4(0.1f);
		lightDesc.direction = Vec3(1.f, -1.f, 1.f);
		light->GetTransform()->SetRotation(lightDesc.direction);
		static_pointer_cast<Light>(light->GetFixedComponent(ComponentType::Light))->SetLightDesc(lightDesc);
		CURSCENE->Add(light);
	}

	// Billboard
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"28. BillboardDemo.fx");
		auto obj = make_shared<GameObject>();
		obj->GetTransform()->SetLocalPosition(Vec3(0.f));
		obj->AddComponent(make_shared<Billboard>());
		{
			// Material
			{
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				auto texture = RESOURCES->Load<Texture>(L"Grass", L"..\\Resources\\Textures\\grass.png");
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

		for (int32 i = 0; i < 10000; i++)
		{
			Vec2 scale = Vec2(1 + rand() % 3, 1 + rand() % 3);
			Vec2 position = Vec2(-100 + rand() % 200, -100 + rand() % 200);

			obj->GetBillboard()->Add(Vec3(position.x, scale.y * 0.5f, position.y), scale);
		}

		CURSCENE->Add(obj);
	}

	// Terrain
	{
		//auto terrainShader = make_shared<Shader>(L"19. RenderDemo.fx");
		{
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(renderShader);
			auto texture = RESOURCES->Load<Texture>(L"TerrainGrass", L"..\\Resources\\Textures\\Terrain\\grass.jpg");
			material->SetDiffuseMap(texture);
			MaterialDesc& desc = material->GetMaterialDesc();
			desc.ambient = Vec4(1.f);
			desc.diffuse = Vec4(1.f);
			desc.specular = Vec4(1.f);
			RESOURCES->Add(L"TerrainGrass", material);
		}
		auto obj = make_shared<GameObject>();
		obj->GetTransform()->SetLocalPosition(Vec3(-100.f, 0.f, -100.f));
		obj->AddComponent(make_shared<Terrain>());
		obj->GetTerrain()->Create(200, 200, RESOURCES->Get<Material>(L"TerrainGrass"));
		CURSCENE->Add(obj);
	}

	//Particle
	{
		auto particleShader = make_shared<Shader>(L"ParticleSystem.fx");
		auto obj = make_shared<GameObject>();
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, 5.f, 0.f));
		obj->AddComponent(make_shared<ParticleSystem>());
		shared_ptr<ParticleSystem> particleSystem = obj->GetFixedComponent<ParticleSystem>(ComponentType::ParticleSystem);
		particleSystem->SetEmitDirW(Vec3(0.f, 2.f, 0.f));
		shared_ptr<Material> material = make_shared<Material>();
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetShader(particleShader);
		auto texture = RESOURCES->Load<Texture>(L"Flare", L"..\\Resources\\Textures\\flare0.png");
		material->SetDiffuseMap(texture);
		material->SetRandomTex(RESOURCES->Get<Texture>(L"RandomTex"));
		particleSystem->SetMaterial(material);
		CURSCENE->Add(obj);
	}

	// SnowBillboard
	{	// Billboard
		{
			auto snowShader = make_shared<Shader>(L"29. SnowBillboard.fx");
			auto obj = make_shared<GameObject>();
			obj->GetTransform()->SetLocalPosition(Vec3(0.f));
			obj->AddComponent(make_shared<SnowBillboard>(Vec3(100, 100, 100), 10000));
			{
				// Material
				{
					shared_ptr<Material> material = make_shared<Material>();
					material->SetShader(snowShader);
					auto texture = RESOURCES->Load<Texture>(L"SnowGrass", L"..\\Resources\\Textures\\grass.png");
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
	}

	{
		// Animation
		shared_ptr<Model> m1 = make_shared<Model>();
		m1->ReadModel(L"Kachujin/Kachujin");
		m1->ReadMaterial(L"Kachujin/Kachujin");
		m1->ReadAnimation(L"Kachujin/Idle");
		m1->ReadAnimation(L"Kachujin/Run");
		m1->ReadAnimation(L"Kachujin/Slash");

		for (int32 i = 0; i < 500; i++)
		{
			auto obj = make_shared<GameObject>();
			obj->GetTransform()->SetPosition(Vec3(rand() % 100, 0, rand() % 100));
			obj->GetTransform()->SetScale(Vec3(0.01f));
			obj->AddComponent(make_shared<ModelAnimator>(renderShader));
			{
				obj->GetModelAnimator()->SetModel(m1);
				obj->GetModelAnimator()->SetPass(2);
			}
			CURSCENE->Add(obj);
		}
	}

	{
		// Model
		shared_ptr<Model> m2 = make_shared<Model>();
		m2->ReadModel(L"Tower/Tower");
		m2->ReadMaterial(L"Tower/Tower");

		for (int32 i = 0; i < 100; i++)
		{
			auto obj = make_shared<GameObject>();
			obj->GetTransform()->SetPosition(Vec3(rand() % 100, -1, rand() % 100));
			obj->GetTransform()->SetScale(Vec3(0.01f));

			obj->AddComponent(make_shared<ModelRenderer>(renderShader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}

			CURSCENE->Add(obj);
		}
	}

	// UI
	{
		const int debugUISize = 5;
		auto obj = make_shared<GameObject>();
		obj->SetLayerIndex(LAYER_UI);
		obj->AddComponent(make_shared<Button>());
		auto material = make_shared<Material>();
		//auto texture = make_shared<Texture>();
		//texture->SetSRV(GRAPHICS->GetShadowMapSRV());
		material->SetDiffuseMap(GRAPHICS->GetShadowMap());
		material->SetShader(make_shared<Shader>(L"DebugTexture.fx"));
		obj->GetButton()->Create(Vec2(2048 / (debugUISize * 2) + 20, 2048 / (debugUISize * 2) + 20), Vec2(2048 / debugUISize, 2048 / debugUISize), material);
		obj->GetButton()->AddOnClikedEvent([obj]() { CURSCENE->Remove(obj); });

		CURSCENE->Add(obj);
	}

	{
		// UICamera
		auto camera = make_shared<GameObject>();
		camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
		camera->GetCamera()->SetNear(1.0f);
		camera->GetCamera()->SetFar(100.0f);
		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, false);
		CURSCENE->Add(camera);
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