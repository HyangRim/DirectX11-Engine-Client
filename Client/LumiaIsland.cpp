#include "pch.h"
#include "LumiaIsland.h"

#include "BillboardDemo.h"
#include "BiancaTest.h"
#include "BiancaCamera.h"
#include "FogOfWar.h"
#include "CameraScript.h"

#include "AnimationStateMachine.h"


#include "BiancaRunState.h"
#include "BiancaQSkillState.h"
#include "BiancaWaitState.h"
#include "BiancaESkillState.h"
#include "BiancaRSkillState.h"

#include "NickyWaitState.h"
#include "NickyRunState.h"
#include "NickyESkillState.h"
#include "NickyWSkillState.h"
#include "NickyQSkillState.h"
#include "NickyRSkillState.h"

#include "NavMesh.h"
#include "NavMeshAgent.h"

void LumiaIsland::Init()
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
		CURSCENE->Add(light);
	}
	//CreateCemeteryBase();
	//CreateCemeteryInterior();
	//CreateCemeteryEnvironment();
	CreateCharacterNicky();
	//CreateCharacterBianca();

	// NavMesh 생성 추가
	CreateNavMesh();
}

void LumiaIsland::Update()
{
	
}

void LumiaIsland::Render()
{
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
	camera->GetCamera()->SetNear(1.0f);
	camera->GetCamera()->SetFar(100.0f);
	camera->GetCamera()->SetCullingMaskAll();
	camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
	CURSCENE->Add(camera);
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
		obj->AddComponent(make_shared<AABBBoxCollider>());
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
		obj->AddComponent(make_shared<AABBBoxCollider>());
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
		obj->AddComponent(make_shared<AABBBoxCollider>());
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
		obj->AddComponent(make_shared<AABBBoxCollider>());
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
		obj->AddComponent(make_shared<AABBBoxCollider>());
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
		obj->AddComponent(make_shared<AABBBoxCollider>());
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
		obj->AddComponent(make_shared<AABBBoxCollider>());
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
		obj->AddComponent(make_shared<AABBBoxCollider>());
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

		m_navMesh->GetTransform()->SetPosition(Vec3(-76.7, 20, -57));
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
	// Animation
	shared_ptr<Model> m1 = make_shared<Model>();

	m1->ReadModel(L"Nicky/Nicky");
	m1->ReadMaterial(L"Nicky/Nicky");



	//대기
	m1->ReadAnimation(L"Wait", L"Nicky/Nicky_Glove_Wait");

	//달리기
	m1->ReadAnimation(L"Run", L"Nicky/Nicky_Glove_Run");

	//평타
	m1->ReadAnimation(L"BaseAttack_01", L"Nicky/Nicky_Glove_Atk_01");
	m1->ReadAnimation(L"BaseAttack_02", L"Nicky/Nicky_Glove_Atk_02");

	////Q
	m1->ReadAnimation(L"Skill_01_Attack", L"Nicky/Nicky_Glove_Skill_01_Attack");
	m1->ReadAnimation(L"Skill_01_Rush", L"Nicky/Nicky_Glove_Skill_01_Rush");
	m1->ReadAnimation(L"Skill_01_End", L"Nicky/Nicky_Glove_Skill_01_End");
	//Q Charge
	m1->ReadAnimation(L"Skill_01_Charge_Loop_Run", L"Nicky/Nicky_Glove_Skill_01_Charge_Loop_Run");
	m1->ReadAnimation(L"Skill_01_Charge_Start_Run", L"Nicky/Nicky_Glove_Skill_01_Charge_Start_Run");
	m1->ReadAnimation(L"Skill_01_Charge_Loop_Wait", L"Nicky/Nicky_Glove_Skill_01_Charge_Loop_Wait");
	m1->ReadAnimation(L"Skill_01_Charge_Start_Wait", L"Nicky/Nicky_Glove_Skill_01_Charge_Start_Wait");

	//W
	m1->ReadAnimation(L"Skill_02_Guard", L"Nicky/Nicky_Glove_Skill_02_Guard");
	m1->ReadAnimation(L"Skill_02_Loop", L"Nicky/Nicky_Glove_Skill_02_Loop");

	//E
	m1->ReadAnimation(L"Skill_03", L"Nicky/Nicky_Glove_Skill_03");

	//R
	m1->ReadAnimation(L"Skill_04_Attack", L"Nicky/Nicky_Glove_Skill_04_Attack");
	m1->ReadAnimation(L"Skill_04_Ready", L"Nicky/Nicky_Glove_Skill_04_Ready");
	m1->ReadAnimation(L"Skill_04_Start", L"Nicky/Nicky_Glove_Skill_04_Start");



	for (int32 i = 0; i < 1; i++)
	{

		nicky = make_shared<GameObject>();
		nicky->SetName(L"Nicky");
		

		nicky->GetTransform()->SetPosition(Vec3(15, 20, 5));
		nicky->GetTransform()->SetScale(Vec3(1.f));

		nicky->AddComponent(make_shared<SphereCollider>());
		nicky->AddComponent(make_shared<Rigidbody>());
		nicky->AddComponent(make_shared<NavMeshAgent>());
		nicky->SetType(OBJECTTYPE::ITEMBOX);

		nicky->GetCollider()->SetOffset(Vec3(0.f, 1.f, 0.f));
		nicky->GetRigidbody()->SetStatic(true);

		nicky->AddComponent(make_shared<ModelAnimator>(m_defaultshader));
		{
			nicky->GetModelAnimator()->SetModel(m1);
			nicky->GetModelAnimator()->SetPass(2);
		}
		
	
		// FSM 추가
		auto stateMachine = make_shared<AnimationStateMachine>();
		nicky->AddComponent(stateMachine);

		nicky->GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait, make_shared<NickyWaitState>());
		nicky->GetAnimationStateMachine()->RegisterState(AnimationStateType::Run, make_shared<NickyRunState>());

		nicky->GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_2, make_shared<NickyWSkillState>());
		nicky->GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_3, make_shared<NickyESkillState>());
		nicky->GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_4, make_shared<NickyRSkillState>());
		nicky->GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_1, make_shared<NickyQSkillState>());


		// 기존 시퀀스 생성 코드는 유지 (필요시 사용)
		auto animator = nicky->GetModelAnimator();
	
		// 평타 시퀀스 (BaseAttack_01 -> BaseAttack_02)
		vector<wstring> baseAttackAnims = { L"BaseAttack_02", L"BaseAttack_01" };
		vector<float> baseAttackDurations = { 0.8f, 1.2f }; 
		animator->CreateSequence(L"BaseAttack_Sequence", baseAttackAnims, baseAttackDurations, false);

		// Q 스킬 시퀀스 (Skill_01_Attack -> Skill_01_Rush -> Skill_01_End)
		vector<wstring> skill1Anims = { L"Skill_01_Attack", L"Skill_01_Rush", L"Skill_01_End" };
		vector<float> skill1Durations = { 0.5f, 1.0f, 0.7f }; 
		animator->CreateSequence(L"Skill_1_Sequence", skill1Anims, skill1Durations, false);

		// W 스킬 시퀀스 (Skill_02_Guard -> Skill_02_Loop)
		vector<wstring> skill2Anims = { L"Skill_02_Guard" };
		animator->CreateSequence(L"Skill_2_Sequence", skill2Anims, false);

		// E 스킬 시퀀스 (Skill_03 단일)
		vector<wstring> skill3Anims = { L"Skill_03" };
		animator->CreateSequence(L"Skill_3_Sequence", skill3Anims, false);

		// R 스킬 시퀀스 (Skill_04_Ready -> Skill_04_Start -> Skill_04_Attack)
		vector<wstring> skill4Anims = { L"Skill_04_Ready", L"Skill_04_Start", L"Skill_01_Rush", L"Skill_04_Attack"};
		vector<float> skill4Durations; 
		skill4Durations.push_back(animator->GetAnimationDuration(L"Skill_04_Ready"));  
		skill4Durations.push_back(animator->GetAnimationDuration(L"Skill_04_Start"));  
		skill4Durations.push_back(3.f); 
		skill4Durations.push_back(animator->GetAnimationDuration(L"Skill_04_Attack"));  
		animator->CreateSequence(L"Skill_4_Sequence", skill4Anims, skill4Durations, false);




		CURSCENE->Add(nicky);
	}
}


void LumiaIsland::CreateCharacterBianca()
{
	//Animation
	shared_ptr<Model> m1 = make_shared<Model>();

	m1->ReadModel(L"Bianca2/Bianca");
	m1->ReadMaterial(L"Bianca2/Bianca");
	m1->ReadAnimation(L"Wait", L"Bianca2/Bianca_wait");
	m1->ReadAnimation(L"Run", L"Bianca2/Bianca_run");

	m1->ReadAnimation(L"Skill_1", L"Bianca2/Bianca_skill1");

	m1->ReadAnimation(L"Skill_3_1", L"Bianca2/Bianca_skill3-1");
	m1->ReadAnimation(L"Skill_3_2", L"Bianca2/Bianca_skill3-2");
	m1->ReadAnimation(L"Skill_3_3", L"Bianca2/Bianca_skill3-3");


	m1->ReadAnimation(L"Skill_4_1", L"Bianca2/Bianca_skill4");
	m1->ReadAnimation(L"Skill_4_2", L"Bianca2/Bianca_skill4-2");



	for (int32 i = 0; i < 1; i++)
	{

		bianca = make_shared<GameObject>();
		bianca->GetTransform()->SetPosition(Vec3(15, 20, 5));
		bianca->GetTransform()->SetScale(Vec3(1.f));

		bianca->AddComponent(make_shared<ModelAnimator>(m_defaultshader));
		{
			bianca->GetModelAnimator()->SetModel(m1);
			bianca->GetModelAnimator()->SetPass(2);
		}
		bianca->AddComponent(make_shared<SphereCollider>());
		bianca->AddComponent(make_shared<Rigidbody>());
		bianca->AddComponent(make_shared<NavMeshAgent>());
		bianca->GetCollider()->SetOffset(Vec3(0.f, 1.f, 0.f));
		bianca->GetRigidbody()->SetStatic(true);
		
		
		auto animator = bianca->GetModelAnimator();
		// FSM 추가
		auto stateMachine = make_shared<AnimationStateMachine>();
		bianca->AddComponent(stateMachine);

		bianca->GetAnimationStateMachine()->RegisterState(AnimationStateType::Wait, make_shared<BiancaWaitState>());
		bianca->GetAnimationStateMachine()->RegisterState(AnimationStateType::Run, make_shared<BiancaRunState>());
		bianca->GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_1, make_shared<BiancaQSkillState>());
		bianca->GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_3, make_shared<BiancaESkillState>());
		bianca->GetAnimationStateMachine()->RegisterState(AnimationStateType::Skill_4, make_shared<BiancaRSkillState>());

		// Q 스킬 시퀀스 
		vector<wstring> skill1Anims = { L"Skill_1" };
		animator->CreateSequence(L"Skill_1_Sequence", skill1Anims, false);

		// R 스킬 시퀀스 (Skill_04_Ready -> Skill_04_Start -> Skill_04_Attack)
		vector<wstring> skill4Anims = { L"Skill_4_1", L"Skill_4_2" };
		animator->CreateSequence(L"Skill_4_Sequence", skill4Anims, false);

		CURSCENE->Add(bianca);

		//camera->GetTransform()->SetParent(obj->GetTransform());
		//auto BiancaCam = make_shared<BiancaCamera>();
		//camera->AddComponent(BiancaCam);
		//BiancaCam->SetTarget(obj);
		//BiancaCam->SetOffset(Vec3(0.f, 12.f, -12.5f));
		//camera->GetTransform()->SetRotation(Vec3{ 45.f, 0.f, 0.f });

	} 
}