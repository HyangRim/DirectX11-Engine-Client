#include "pch.h"
#include "LumiaIsland.h"

#include "BillboardDemo.h"
#include "BiancaTest.h"
#include "BiancaCamera.h"
#include "FogOfWar.h"
#include "CameraScript.h"

#include "AnimationStateMachine.h"
#include "SkillDecalIndicator.h"


#include "Bianca.h"
#include "Nicky.h"

#include "NickyESkillState.h"
#include "NickyQSkillState.h"
#include "NickyRSkillState.h"
#include "NickyMoveState.h"
#include "NickyRunState.h"
#include "NickyWaitState.h"
#include "NickyWSkillState.h"

#include "NavMesh.h"
#include "NavMeshAgent.h"

const vector<wstring> charStatIconNames = {
	L"AttackPower",
	L"SkillAmpRatio",
	L"IncreaseBasicAttackDamageRatio",
	L"Defense",
	L"AttackSpeedRatio",
	L"CooldownReduction",
	L"CriticalStrikeChance",
	L"MoveSpeedRatio"
};

void LumiaIsland::Start()
{
	m_defaultshader = make_shared<Shader>(L"FOW.fx");
	//m_testShader = make_shared<Shader>(L"23. RenderDemo.fx");
	//CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\snowcube1024.dds", L"Sky.fx"));
	
	CreateMainCamera();
	CreateUICamera();


	//Default Light

	{
		// Light
		auto light = make_shared<GameObject>();
		light->AddComponent(make_shared<Light>());

		LightDesc lightDesc;
		lightDesc.ambient = Vec4(0.4f);
		lightDesc.diffuse = Vec4(1.f);
		lightDesc.specular = Vec4(0.1f);
		lightDesc.direction = Vec3(1.f, 1.f, 1.f);
		//light->GetLight()->SetLightDesc(lightDesc);
		light->GetTransform()->SetRotation(lightDesc.direction);
		light->GetTransform()->SetPosition(Vec3(0.f, 150.f, 0.f));
		static_pointer_cast<Light>(light->GetFixedComponent(ComponentType::Light))->SetLightDesc(lightDesc);
		Add(light);
	}
	CreateCemeteryBase();
	CreateCemeteryInterior();
	CreateCemeteryEnvironment();
	//CreateCharacterNicky();
	CreateCemeteryItemBox();
	CreateCharacterBianca();
	CreateTestDummy();
	//CreateTestDecal();

	// NavMesh 생성 추가
	CreateNavMesh();

	// Billboard
	{
		auto snowShader = make_shared<Shader>(L"GatherBillboard.fx");
		auto obj = make_shared<GameObject>();
		obj->SetType(OBJECTTYPE::MAP);
		obj->GetTransform()->SetLocalPosition(Vec3(15, 20, 5));
		obj->AddComponent(make_shared<SnowBillboard>(Vec3(15, 0, 5), Vec3(3, 3, 3), 50));
		{
			// Material
			{
				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(snowShader);
				//auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\grass.png");
				auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
				material->SetDiffuseMap(texture);
				MaterialDesc& desc = material->GetMaterialDesc();
				desc.ambient = Vec4(1.f);
				desc.diffuse = Vec4(1.f);
				desc.specular = Vec4(1.f);
				RESOURCES->Add(L"Veigar", material);

				obj->GetSnowBillboard()->SetMaterial(material);
				obj->GetSnowBillboard()->SetParticleScale(Vec2(0.3f, 0.3f));
			}
		}

		Add(obj);
	}


	//{
	//	auto obj = make_shared<GameObject>();
	//	obj->SetName(L"CONE");
	//	//obj->SetType(OBJECTTYPE::MAP);
	//	obj->GetTransform()->SetLocalPosition(Vec3(15.f, 20.f, 25.f));
	//	obj->GetTransform()->SetScale(Vec3(1.f));
	//	obj->AddComponent(make_shared<MeshRenderer>());
	//	{
	//		obj->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"default"));
	//	}
	//	{
	//		auto mesh = RESOURCES->Get<Mesh>(L"Cone");
	//		obj->GetMeshRenderer()->SetMesh(mesh);
	//		obj->GetMeshRenderer()->SetPass(0);
	//	}
	//	obj->AddComponent(make_shared<AABBBoxCollider>());
	//	Add(obj);
	//}



	//====================UI====================//
	LoadItemBoxImages();
	LoadCharStatIcon();
	LoadCharEquipmentIcon();
	LoadCharMainImages();
	LoadCharInventoryImages();
	
	
	CreateItemBoxPanel();
	CreateCharStatPanel();
	CreateCharEquipmentPanel();
	CreateCharMainPanel();
	CreateCharInventoryPanel();
	//====================UI====================//

	//CreateTestMesh();

	Super::Start();
}

void LumiaIsland::Update()
{
	Super::Update();

	
	CheckPickedItemBox();
}

void LumiaIsland::FixedUpdate()
{
	Super::FixedUpdate();
}

void LumiaIsland::LateUpdate()
{
	Super::LateUpdate();
}

void LumiaIsland::Render()
{
	Super::Render();
}

void LumiaIsland::CreateMainCamera()
{
	// Camera
	auto camera = make_shared<GameObject>();
	//camera->GetTransform()->SetPosition(Vec3(0.f, 15.f, 15.f));
	camera->GetTransform()->SetPosition(Vec3{ 0.f, 30.f, -5.f });
	camera->AddComponent(make_shared<Camera>());
	camera->AddComponent(make_shared<CameraScript>());

	camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
	CURSCENE->Add(camera);
}

void LumiaIsland::CreateUICamera()
{
	// UICamera
	auto camera = make_shared<GameObject>();
	camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	camera->AddComponent(make_shared<Camera>());
	camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
	camera->GetCamera()->SetNear(0.1f);
	camera->GetCamera()->SetFar(100.0f);
	camera->GetCamera()->SetCullingMaskAll();
	camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, false);
	//CURSCENE->Add(camera);
	Add(camera);
}

void LumiaIsland::CreateCemeteryBase()
{
	m_CemeteryParent = make_shared<GameObject>();
	m_CemeteryParent->SetName(L"Cemetery_Parent");
	CURSCENE->Add(m_CemeteryParent);

	shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_Base", L"Cemetery/Cemetery_STR_Base");
	//m2->ReadModel(L"forest/forest");
	m2->ReadMaterial(L"Cemetery/Cemetery_STR_Base");
	auto obj = make_shared<GameObject>();
	obj->SetName(L"Cemetery_STR_Base");
	obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
	obj->GetTransform()->SetLocalPosition(Vec3(0, 0, 0));
	obj->GetTransform()->SetLocalRotation(Vec3(0, 0.f, 0));
	obj->AddComponent(make_shared<SphereCollider>());
	obj->GetTransform()->SetLocalScale(Vec3(0.02f));
	obj->SetType(OBJECTTYPE::MAP);

	obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
	{
		obj->GetModelRenderer()->SetModel(m2);
		obj->GetModelRenderer()->SetPass(1);
	}

	CURSCENE->Add(obj);
}

void LumiaIsland::CreateCemeteryInterior()
{
	//OuterWall
	{
		//Cemetery_STR_OuterWall_02
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_OuterWall_02", L"Cemetery/Cemetery_STR_OuterWall_02");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_OuterWall_02");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_OuterWall_02");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(125.919, 17.859, 105.524));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_OuterWall_02_Fence
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_OuterWall_02_Fence", L"Cemetery/Cemetery_STR_OuterWall_02_Fence");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_OuterWall_02_Fence");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_OuterWall_02_Fence");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(125.919, 17.859, 105.524));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_GraveBase_02
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_GraveBase_02", L"Cemetery/Cemetery_STR_GraveBase_02");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_GraveBase_02");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_GraveBase_02");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(125.919, 17.859, 105.524));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_OuterWall_02_Fence
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_OuterWall_02_Grass", L"Cemetery/Cemetery_STR_OuterWall_02_Grass");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_OuterWall_02_Grass");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_OuterWall_02_Grass");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(125.919, 18.1, 105.524));
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}


		//Cemetery_STR_InnerWall_02
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_InnerWall_02", L"Cemetery/Cemetery_STR_InnerWall_02");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_InnerWall_02");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_InnerWall_02");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(125.919, 17.859, 105.524));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}
	}


	{
		//Cemetery_STR_OuterWall_03
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_OuterWall_03", L"Cemetery/Cemetery_STR_OuterWall_03");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_OuterWall_03");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_OuterWall_03");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(158.7, 17.859, 63.083));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_OuterWall_03_Fence
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_OuterWall_03_Fence", L"Cemetery/Cemetery_STR_OuterWall_03_Fence");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_OuterWall_03_Fence");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_OuterWall_03_Fence");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(158.7, 17.859, 63.083));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_House_01_Wall
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_House_01_Wall", L"Cemetery/Cemetery_STR_House_01_Wall");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_House_01_Wall");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_House_01_Wall");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(142.178, 17.859, 29.834));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_House_01
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_House_01", L"Cemetery/Cemetery_STR_House_01");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_House_01");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_House_01");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(142.178, 17.859, 29.834));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_House_01_Interior
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_House_01_Interior", L"Cemetery/Cemetery_STR_House_01_Interior");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_House_01_Interior");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_House_01_Interior");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(142.178, 17.859, 29.834));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_House_01_Wall_Grass
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_House_01_Wall_Grass", L"Cemetery/Cemetery_STR_House_01_Wall_Grass");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_House_01_Wall_Grass");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_House_01_Wall_Grass");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(142.178, 17.1, 29.834));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

	}
	// TextDeco_Set
	{
		//Bg_Cemetery_STR_TextDeco_Set
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Bg_Cemetery_STR_TextDeco_Set", L"Cemetery/Bg_Cemetery_STR_TextDeco_Set");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Bg_Cemetery_STR_TextDeco_Set");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Bg_Cemetery_STR_TextDeco_Set");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(114, 24.7, 26.665));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Bg_Cemetery_STR_TextDeco_Set_01
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Bg_Cemetery_STR_TextDeco_Set", L"Cemetery/Bg_Cemetery_STR_TextDeco_Set");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Bg_Cemetery_STR_TextDeco_Set");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Bg_Cemetery_STR_TextDeco_Set_01");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(120.843, 24.965, 113));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}
	}


	//Cemetery_STR_OuterWall_04
	{
		//Cemetery_STR_OuterWall_04
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_OuterWall_04", L"Cemetery/Cemetery_STR_OuterWall_04");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_OuterWall_04");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_OuterWall_04");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(64.588, 17.859, 26.163));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_OuterWall_04_Fence
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_OuterWall_04_Fence", L"Cemetery/Cemetery_STR_OuterWall_04_Fence");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_OuterWall_04_Fence");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_OuterWall_04_Fence");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(64.588, 17.859, 26.163));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}
	}

	//Cemetery_STR_GrabeBase_03_Wall_01
	{

		//Cemetery_STR_GrabeBase_03_Wall_01
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_GraveBase_03_Wall_01", L"Cemetery/Cemetery_STR_GraveBase_03_Wall_01");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_01");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_GraveBase_03_Wall_01");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(92, 17.859, 55.934));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_GraveBase_03_Wall_01_Fence
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_GraveBase_03_Wall_01_Fence", L"Cemetery/Cemetery_STR_GraveBase_03_Wall_01_Fence");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_01_Fence");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_GraveBase_03_Wall_01_Fence");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(92, 17.859, 55.934));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_GraveBase_03_Wall_02
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_GraveBase_03_Wall_02", L"Cemetery/Cemetery_STR_GraveBase_03_Wall_02");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_02");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_GraveBase_03_Wall_02");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(92, 17.429, 55.934));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_GraveBase_03_Wall_02_01
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_GraveBase_03_Wall_02", L"Cemetery/Cemetery_STR_GraveBase_03_Wall_02");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_02");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_GraveBase_03_Wall_02");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(77.3, 17.429, 55.934));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}
	}


	//ETC
	{
		//Cemetery_STR_InnerWall_04
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_InnerWall_04", L"Cemetery/Cemetery_STR_InnerWall_04");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_InnerWall_04");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_InnerWall_04");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(78.7, 17.865, 5.23));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}

		//Cemetery_STR_InnerWall_05
		{
			shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_STR_InnerWall_05", L"Cemetery/Cemetery_STR_InnerWall_05");
			//m2->ReadModel(L"forest/forest");
			m2->ReadMaterial(L"Cemetery/Cemetery_STR_InnerWall_05");
			auto obj = make_shared<GameObject>();
			obj->SetName(L"Cemetery_STR_InnerWall_05");
			obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
			obj->GetTransform()->SetLocalPosition(Vec3(78.7, 17.865, 5.23));
			//obj->AddComponent(make_shared<AABBBoxCollider>());
			obj->GetTransform()->SetLocalScale(Vec3(0.02f));
			obj->SetType(OBJECTTYPE::MAP);

			obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
			{
				obj->GetModelRenderer()->SetModel(m2);
				obj->GetModelRenderer()->SetPass(1);
			}
			CURSCENE->Add(obj);
		}
	}
}

void LumiaIsland::CreateCemeteryEnvironment()
{
	//가운데 기준 11시쪽 무덤 - 1
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_OBJ_Tombstone_02", L"Cemetery/Cemetery_OBJ_Tombstone_02");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/Cemetery_OBJ_Tombstone_02");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"Cemetery_OBJ_Tombstone_01");
		obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(96.002, 20, 79.367));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 5486.223f, 0.f));
		//obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ENVIRONMENT);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}



	//가운데 기준 5시쪽 무덤 - 1
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_OBJ_Tombstone_02", L"Cemetery/Cemetery_OBJ_Tombstone_02");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/Cemetery_OBJ_Tombstone_02");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"Cemetery_OBJ_Tombstone_01");
		obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(124.576, 20.117, 44.456));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 2557.334, 0.f));
		//obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ENVIRONMENT);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}


	//가운데 기준 11시쪽 무덤 - 2
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_OBJ_Tombstone_03", L"Cemetery/Cemetery_OBJ_Tombstone_03");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/Cemetery_OBJ_Tombstone_03");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"Cemetery_OBJ_Tombstone_03");
		obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(83.649, 20, 79.549));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 4395.447, 0.f));
		//obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ENVIRONMENT);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}



	//가운데 기준 5시시쪽 무덤 - 2
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_OBJ_Tombstone_04", L"Cemetery/Cemetery_OBJ_Tombstone_04");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/Cemetery_OBJ_Tombstone_04");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"Cemetery_OBJ_Tombstone_04");
		obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(120.567, 20, 44.595));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, -1263.546, 0.f));
		//obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ENVIRONMENT);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}

	//가운데 기준 5시시쪽 무덤 - 3
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_OBJ_Tombstone_04", L"Cemetery/Cemetery_OBJ_Tombstone_04");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/Cemetery_OBJ_Tombstone_04");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"Cemetery_OBJ_Tombstone_04");
		obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(110.770, 20, 44.711));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
		//obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ENVIRONMENT);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}




	//가운데 기준 1시시쪽 무덤 - 1
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_OBJ_Tomb_02", L"Cemetery/Cemetery_OBJ_Tomb_02");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/Cemetery_OBJ_Tomb_02");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"Cemetery_OBJ_Tomb_02");
		obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(124.497, 20, 69.716));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, -47.64, 0.f));
		//obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ENVIRONMENT);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}

	//가운데 기준 7시시쪽 무덤 - 1
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_OBJ_Tomb_03", L"Cemetery/Cemetery_OBJ_Tomb_03");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/Cemetery_OBJ_Tomb_03");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"Cemetery_OBJ_Tomb_03");
		obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(83.009, 21.389, 45.124));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 169, 0.f));
		//obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ENVIRONMENT);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}


	//가운데 기준 11시시쪽 무덤 - 3
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Cemetery_OBJ_Tomb_03", L"Cemetery/Cemetery_OBJ_Tomb_03");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/Cemetery_OBJ_Tomb_03");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"Cemetery_OBJ_Tomb_03");
		obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(83.232, 20, 71.818));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 169, 0.f));
		//obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ENVIRONMENT);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}
}

void LumiaIsland::CreateCemeteryItemBox()
{
	//가운데 기준 11시시쪽 무덤 - 3
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"ItemBox", L"Cemetery/ItemBox");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/ItemBox");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"ItemBox_01");
		//obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(96.9, 19.5, 59));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
		obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ITEMBOX);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}

	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"ItemBox", L"Cemetery/ItemBox");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/ItemBox");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"ItemBox_02");
		//obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(96.9, 20, 65.94));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 180.f, 0.f));
		obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ITEMBOX);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}

	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"ItemBox", L"Cemetery/ItemBox");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/ItemBox");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"ItemBox_03");
		//obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(111.6, 19.5, 59));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
		obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ITEMBOX);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}

	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"ItemBox", L"Cemetery/ItemBox");
		//m2->ReadModel(L"forest/forest");
		m2->ReadMaterial(L"Cemetery/ItemBox");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"ItemBox_04");
		//obj->GetTransform()->SetParent(m_CemeteryParent->GetTransform());
		obj->GetTransform()->SetLocalPosition(Vec3(111.6, 20, 65.94));
		obj->GetTransform()->SetLocalRotation(Vec3(0.f, 180.f, 0.f));
		obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(0.02f));
		obj->SetType(OBJECTTYPE::ITEMBOX);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}

}

void LumiaIsland::CreateNavMesh()
{
	// Animation
	shared_ptr<Model> m1 = make_shared<Model>();

	m1->ReadModel(L"NavMesh/NavMesh");
	m1->ReadMaterial(L"NavMesh/NavMesh");


	for (int32 i = 0; i < 1; i++)
	{

		m_navMesh = make_shared<GameObject>();
		m_navMesh->SetName(to_wstring(i));

		m_navMesh->GetTransform()->SetPosition(Vec3(-76.7, 18, -57));
		m_navMesh->GetTransform()->SetScale(Vec3(2.f));
		m_navMesh->GetTransform()->SetLocalRotation(Vec3(270.f, 90.f, 90.f));

		m_navMesh->AddComponent(make_shared<SphereCollider>());
		m_navMesh->AddComponent(make_shared<Rigidbody>());
		m_navMesh->GetCollider()->SetOffset(Vec3(0.f, 1.f, 0.f));
		m_navMesh->GetRigidbody()->SetStatic(true);
		m_navMesh->SetType(OBJECTTYPE::MAP);

		m_navMesh->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			m_navMesh->GetModelRenderer()->SetModel(m1);
			m_navMesh->GetModelRenderer()->SetPass(3);
		}

		m_navMesh->AddComponent(make_shared<NavMesh>());
		
	
		CURSCENE->Add(m_navMesh);
	}
}

void LumiaIsland::CreateCharacterNicky()
{
	shared_ptr<Nicky> nicky = make_shared<Nicky>(m_defaultshader);
	nicky->GetTransform()->SetPosition(Vec3(15, 18, 5));
	nicky->GetTransform()->SetScale(Vec3(1.f));
	CURSCENE->Add(nicky);
}


void LumiaIsland::CreateCharacterBianca()
{
	shared_ptr<Bianca> bianca = make_shared<Bianca>(m_defaultshader);
	bianca->GetTransform()->SetPosition(Vec3(15, 18, 5));
	bianca->GetTransform()->SetScale(Vec3(1.f));
	CURSCENE->Add(bianca);
}

void LumiaIsland::LoadItemBoxImages()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	wstring prefixPath = L"..\\Resources\\Textures\\UI\\ItemBox_UI\\";

	shared_ptr<Material> itemBoxPanel = make_shared<Material>();
	SetupUIMaterial(itemBoxPanel);
	auto itemBoxPanelTexture = RESOURCES->Load<Texture>(L"ItemBoxPanel", prefixPath + L"ItemBox_BackGround.png");
	itemBoxPanel->SetDiffuseMap(itemBoxPanelTexture);
	MaterialDesc& itemBoxPanelDesc = itemBoxPanel->GetMaterialDesc();
	itemBoxPanelDesc.ambient = Vec4(1.f);
	itemBoxPanelDesc.diffuse = Vec4(1.f);
	itemBoxPanelDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"ItemBoxPanel", itemBoxPanel);

	shared_ptr<Material> itemSlotCommon = make_shared<Material>();
	SetupUIMaterial(itemSlotCommon);
	auto itemSlotCommonTexture = RESOURCES->Load<Texture>(L"ItemSlotCommon", prefixPath + L"Img_Item_Slot_Common.png");
	itemSlotCommon->SetDiffuseMap(itemSlotCommonTexture);
	MaterialDesc& itemSlotCommonDesc = itemSlotCommon->GetMaterialDesc();
	itemSlotCommonDesc.ambient = Vec4(1.f);
	itemSlotCommonDesc.diffuse = Vec4(1.f);
	itemSlotCommonDesc.specular = Vec4(1.0f);
	RESOURCES->Add(L"ItemSlotCommon", itemSlotCommon);
}

void LumiaIsland::CreateItemBoxPanel()
{
	m_itemBox = make_shared<GameObject>();
	m_itemBox->SetName(L"ItemBoxPanel");

	auto panel = make_shared<UIPanel>();
	m_itemBox->AddComponent(panel);

	shared_ptr<Material> itemPanelBackGround = RESOURCES->Get<Material>(L"ItemBoxPanel")->Clone();
	panel->Create(Vec2(200.f, 200.f), Vec2(221, 117), itemPanelBackGround);
	m_itemBox->SetLayerIndex(LAYER_UI);

	m_itemBox->GetMeshRenderer()->SetActive(false);

	AddUIObject(m_itemBox, true);
	RegisterUIParent(m_itemBox);
}

void LumiaIsland::CheckPickedItemBox()
{
	if (m_pickedObject != nullptr)
	{
		if (m_pickedObject->GetType() == OBJECTTYPE::ITEMBOX)
		{
			m_itemBox->GetMeshRenderer()->SetActive(true);
			//cout << "아이템박스 클릭됨\n";
		}
		else
		{
			m_itemBox->GetMeshRenderer()->SetActive(false);
			//cout << "아이템박스 클릭해제됨\n";
		}
	}
	else
	{
		m_itemBox->GetMeshRenderer()->SetActive(false);
		//cout << "선택된 객체가 없음\n";
	}
}

void LumiaIsland::LoadCharStatIcon()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	wstring prefixTag = L"Ico_ChaStat_";
	wstring prefixPath = L"..\\Resources\\Textures\\UI\\CharStatIcon\\";

	vector<Vec4> statIconColor;
	statIconColor.push_back(ColorNormalize(Vec4(218, 187, 102, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(211, 160, 221, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(209, 120, 66, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(124, 175, 203, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(171, 162, 118, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(200, 200, 200, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(236, 96, 113, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(200, 200, 200, 255)));

	for (int i = 0; i < charStatIconNames.size(); i++)
	{
		shared_ptr<Material> charLobbyImage = make_shared<Material>();
		SetupUIMaterial(charLobbyImage);

		wstring tag = prefixTag + charStatIconNames[i];
		wstring path = prefixPath + tag + L".png";
		auto charLobbyTexture = RESOURCES->Load<Texture>(tag, path);

		charLobbyImage->SetDiffuseMap(charLobbyTexture);
		MaterialDesc& charLobbyDesc = charLobbyImage->GetMaterialDesc();
		charLobbyDesc.ambient = Vec4(1.f);
		charLobbyDesc.diffuse =	statIconColor[i];
		charLobbyDesc.specular = Vec4(1.0f);
		RESOURCES->Add(tag, charLobbyImage);
	}
}

void LumiaIsland::CreateCharStatPanel()
{
	m_charStatPanel = make_shared<GameObject>();
	m_charStatPanel->SetName(L"CharStatPanel");

	auto panel = make_shared<UIPanel>();
	m_charStatPanel->AddComponent(panel);

	panel->Create(Vec2(274.f, 768 - 57), Vec2(155, 115), nullptr);
	m_charStatPanel->SetLayerIndex(LAYER_UI);

	auto imageUI = m_charStatPanel->GetUIPanel()->AddImageUI(Vec2(0, 0), L"ImageUI");

	for (int i = 0; i < charStatIconNames.size(); i++)
	{
		wstring prefixTag = L"Ico_ChaStat_";
		shared_ptr<Material> cloneMaterial_charStatIcon = RESOURCES->Get<Material>(prefixTag + charStatIconNames[i])->Clone();
		imageUI->AddImageLayer(i, Vec2(16 + (i % 2) * 70, 13 + (i / 2) * 28), Vec2(17, 17), cloneMaterial_charStatIcon, 5);
	}

	AddUIObject(m_charStatPanel, true);
	RegisterUIParent(m_charStatPanel);
}

void LumiaIsland::LoadCharEquipmentIcon()
{

}

void LumiaIsland::CreateCharEquipmentPanel()
{
	m_charEquipPanel = make_shared<GameObject>();
	m_charEquipPanel->SetName(L"CharEquipPanel");

	auto panel = make_shared<UIPanel>();
	m_charEquipPanel->AddComponent(panel);

	panel->Create(Vec2(380.f, 768 - 57), Vec2(38, 115), nullptr);
	m_charEquipPanel->SetLayerIndex(LAYER_UI);

	AddUIObject(m_charEquipPanel, true);
	RegisterUIParent(m_charEquipPanel);
}

void LumiaIsland::LoadCharMainImages()
{

}

void LumiaIsland::CreateCharMainPanel()
{
	m_charMainPanel = make_shared<GameObject>();
	m_charMainPanel->SetName(L"CharMainPanel");

	auto panel = make_shared<UIPanel>();
	m_charMainPanel->AddComponent(panel);

	panel->Create(Vec2(615.f, 768 - 57), Vec2(414, 115), nullptr);
	m_charMainPanel->SetLayerIndex(LAYER_UI);

	AddUIObject(m_charMainPanel, true);
	RegisterUIParent(m_charMainPanel);
}

void LumiaIsland::LoadCharInventoryImages()
{

}

void LumiaIsland::CreateCharInventoryPanel()
{
	m_charInventoryPanel = make_shared<GameObject>();
	m_charInventoryPanel->SetName(L"CharMainPanel");

	auto panel = make_shared<UIPanel>();
	m_charInventoryPanel->AddComponent(panel);

	panel->Create(Vec2(960.f, 768 - 57), Vec2(263, 115), nullptr);
	m_charInventoryPanel->SetLayerIndex(LAYER_UI);

	AddUIObject(m_charInventoryPanel, true);
	RegisterUIParent(m_charInventoryPanel);
}

Vec4 LumiaIsland::ColorNormalize(Vec4 input)
{
	return input / 255.f;
}

void LumiaIsland::CreateTestDecal()
{
	auto testDecalObj = make_shared<GameObject>();
	testDecalObj->AddComponent(make_shared<AABBBoxCollider>());
	testDecalObj->SetName(L"TestDecal");
	testDecalObj->SetType(OBJECTTYPE::MAP);

	// 2. SkillDecalIndicator 컴포넌트만 추가
	auto decalIndicator = make_shared<SkillDecalIndicator>();
	testDecalObj->AddComponent(decalIndicator);
	testDecalObj->GetTransform()->SetLocalPosition(Vec3(15, 25, 10));
	// 3. 간단한 설정
	decalIndicator->SetSkillDecal(SkillDecalType::CIRCLE, 5.0f);
	decalIndicator->SetColor(Vec4(1.0f, 1.0f, 1.0f, 0.6f));
	decalIndicator->SetStartPosition(Vec3(15, 20, 5));
	decalIndicator->ShowIndicator(true);

	// 4. Scene에 추가
	CURSCENE->Add(testDecalObj);
}

void LumiaIsland::CreateTestMesh()
{
	// Animation
	shared_ptr<Model> m1 = make_shared<Model>();

	m1->ReadModel(L"Nicky/NickyESkill_Mesh");
	m1->ReadMaterial(L"Nicky/NickyESkill_Mesh");

	shared_ptr<GameObject> test;

	for (int32 i = 0; i < 1; i++)
	{

		test = make_shared<GameObject>();
		test->SetName(to_wstring(i));

		test->GetTransform()->SetPosition(Vec3(15, 30, 5));
		test->GetTransform()->SetScale(Vec3(0.01f, 0.01f, 0.01f));
		test->GetTransform()->SetLocalRotation(Vec3(90.f, 0.f, 0.f));

		test->AddComponent(make_shared<SphereCollider>());
		test->AddComponent(make_shared<Rigidbody>());
		test->GetCollider()->SetOffset(Vec3(0.f, 1.f, 0.f));
		test->GetRigidbody()->SetStatic(true);
		test->SetType(OBJECTTYPE::MAP);

		test->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			test->GetModelRenderer()->SetModel(m1);
			test->GetModelRenderer()->SetPass(1);
		}

		test->AddComponent(make_shared<NavMesh>());


		CURSCENE->Add(test);
	}
}

void LumiaIsland::CreateTestDummy()
{
	{
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Nicky", L"Nicky/Nicky");
		m2->ReadMaterial(L"Nicky/Nicky");
		auto obj = make_shared<GameObject>();
		obj->SetName(L"TestDummy");
		obj->GetTransform()->SetLocalPosition(Vec3(10, 18, 15));
		obj->AddComponent(make_shared<AABBBoxCollider>());
		obj->GetCollider()->SetOffsetScale(Vec3(1, 1, 1));
		obj->GetTransform()->SetLocalScale(Vec3(1.f));
		obj->SetType(OBJECTTYPE::PLAYER);

		obj->AddComponent(make_shared<ModelRenderer>(m_defaultshader));
		{
			obj->GetModelRenderer()->SetModel(m2);
			obj->GetModelRenderer()->SetPass(1);
		}
		CURSCENE->Add(obj);
	}
}
