#include "pch.h"

#include "LumiaIsland.h"

#include "Cursor.h"

#include "BillboardDemo.h"
#include "BiancaTest.h"
#include "BiancaCamera.h"
#include "FogOfWar.h"
#include "CameraScript.h"

#include "AnimationStateMachine.h"
#include "SkillDecalIndicator.h"

#include "Player.h"

#include "Bianca.h"
#include "Nicky.h"

#include "Wolf.h"
#include "Alpha.h"

#include "NavMesh.h"
#include "NavMeshAgent.h"

#include "D2DText.h"
#include "ISkill.h"

#include "EquipableItem.h"
#include <string>
#include "GameHUDPanelUI.h"

#include "ItemManager.h"
#include "ItemBox.h"
#include "InventoryManager.h"

#include "Recipe.h"
#include "RecipeManager.h"

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

const vector<wstring> nickySkillIcons = {
	L"SkillIcon_1033100",
	L"SkillIcon_1033200",
	L"SkillIcon_1033300",
	L"SkillIcon_1033400",
	L"SkillIcon_1033500"
};

const vector<wstring> biancaSkillIcons = {
	L"SkillIcon_1042100",
	L"SkillIcon_1042200",
	L"SkillIcon_1042300",
	L"SkillIcon_1042400",
	L"SkillIcon_1042500"
};


LumiaIsland::LumiaIsland()
{
	
}

LumiaIsland::~LumiaIsland()
{
	if (m_loadingThread) {
		WaitForSingleObject(m_loadingThread, INFINITE);
		CloseHandle(m_loadingThread);
	}

	DeleteCriticalSection(&m_loadingCS);
	DeleteCriticalSection(&m_mainThreadTasksCS);
}

void LumiaIsland::Start()
{
	

	TIME->ResetDeltaTime();


	InitializeCriticalSection(&m_loadingCS);
	InitializeCriticalSection(&m_mainThreadTasksCS);

	m_defaultshader = make_shared<Shader>(L"FOW.fx");
	//CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\skyBox.png", L"Sky.fx"));
	//m_testShader = make_shared<Shader>(L"23. RenderDemo.fx");
	CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\skybox.dds", L"Sky.fx"));
	
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
		Vec3 lightDirection = Vec3(1.f, -1.f, 1.f); // Y를 음수로 (아래쪽을 향하도록)
		lightDirection.Normalize();
		lightDesc.direction = lightDirection;
		//light->GetLight()->SetLightDesc(lightDesc);
		light->GetTransform()->SetPosition(Vec3(0.f, 150.f, 0.f));
		Vec3 normalizedDir = Vec3(1.f, -1.f, 1.f);
		normalizedDir.Normalize();
		lightDesc.direction = normalizedDir;
		//light->GetTransform()->SetRotation(lightDesc.direction);
		//light->GetTransform()->SetPosition(Vec3(0.f, 150.f, 0.f));

		static_pointer_cast<Light>(light->GetFixedComponent(ComponentType::Light))->SetLightDesc(lightDesc);
		Add(light);
	}

	cout << "LumiaIsland SelectedCharIndex : " << m_selectedCharacterIdx << endl;
	m_selectedCharacterIdx = 1;
	if (m_selectedCharacterIdx == 0) {
		CreateCharacterBianca();
	}
	else if (m_selectedCharacterIdx == 1) {
		CreateCharacterNicky();
	}
	m_uiManager = make_shared<UIManager>(m_player, m_selectedCharacterIdx); //플레이가 존재할때 선언


	m_loadingThread = CreateThread(nullptr, 0, BackgroundLoadingThread, this, 0, nullptr);

	
	CreateCemeteryBase();
	CreateCemeteryInterior();
	CreateCemeteryEnvironment();

	
	m_cameraScript->SetTarget(m_player);
	//CreateCharacterNicky();
	CreateCemeteryItemBox();
	CreateTestDummy();
	//CreateTestDecal();

	//// NavMesh 생성 추가
	CreateNavMesh();


	////Monster 추가.
	//CreateMonsterWolf(Vec3(15, 18, 16));
	//CreateMonsterAlpha(Vec3(20, 18, 16));

	

	//CreateTestMesh();

	TIME->ResetDeltaTime();
	SOUND->StopAll();
	SOUND->PlayBGM(L"BSER_AreaBGM_CEMETERY.wav", 0.5f);
	CreateCursor();
	Super::Start();
}

void LumiaIsland::Update()
{
	ProcessMainThreadTasks();

	Super::Update();


	if (m_objectsCreated) {
		m_uiManager->Update();


		CheckPickedItemBox();
		ControlPlayerStatus();
	
	}
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
	//camera->GetTransform()->SetPosition(Vec3{ 10.f, 30.f, -5.f });
	camera->GetTransform()->SetRotation(Vec3(45.f, -45.f, 0.f));
	camera->AddComponent(make_shared<Camera>());
	//camera->AddComponent(make_shared<CameraScript>());
	m_cameraScript = make_shared<BiancaCamera>();
	camera->AddComponent(m_cameraScript);

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
		obj->AddComponent(make_shared<ItemBox>());
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
		obj->AddComponent(make_shared<ItemBox>());
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
		obj->AddComponent(make_shared<ItemBox>());
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
		obj->AddComponent(make_shared<ItemBox>());
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

		m_navMesh->GetTransform()->SetPosition(Vec3(-75.7, 18, -54));
		//m_navMesh->GetTransform()->SetPosition(Vec3(0, 18, 0));
		m_navMesh->GetTransform()->SetScale(Vec3(2.f));
		m_navMesh->GetTransform()->SetLocalRotation(Vec3(270.f, 270.f, 90.f));

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
	nicky->SetName(L"Nicky");
	nicky->GetTransform()->SetPosition(Vec3(15, 18, 5));
	nicky->GetTransform()->SetScale(Vec3(2.f));
	
	m_selectedCharacterIdx = 1;

	m_player = nicky;

	CURSCENE->Add(nicky);
}


void LumiaIsland::CreateCharacterBianca()
{
	shared_ptr<Bianca> bianca = make_shared<Bianca>(m_defaultshader);
	bianca->GetTransform()->SetPosition(Vec3(15, 18, 5));
	bianca->GetTransform()->SetScale(Vec3(2.f));

	m_selectedCharacterIdx = 0;

	m_player = bianca;


	CURSCENE->Add(bianca);
}

void LumiaIsland::CreateMonsterWolf(Vec3 _pos)
{
	shared_ptr<Wolf> wolf = make_shared<Wolf>(m_defaultshader);

	wolf->GetTransform()->SetPosition(_pos);
	wolf->GetTransform()->SetScale(Vec3(1.f));
	CURSCENE->Add(wolf);

}

void LumiaIsland::CreateMonsterAlpha(Vec3 _pos)
{
	shared_ptr<Alpha> alpha = make_shared<Alpha>(m_defaultshader);

	alpha->GetTransform()->SetPosition(_pos);
	alpha->GetTransform()->SetScale(Vec3(1.f));
	CURSCENE->Add(alpha);
}

void LumiaIsland::CreateCursor()
{
	auto cursorObj = make_shared<GameObject>();
	cursorObj->SetName(L"MouseCursorObject");

	m_cursor = make_shared<Cursor>();
	cursorObj->AddComponent(m_cursor);

	CURSCENE->Add(cursorObj);
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
	panel->Create(Vec2(200.f, 200.f), Vec2(221, 117), Vec4(0.f), itemPanelBackGround);
	m_itemBox->SetLayerIndex(LAYER_UI);

	m_itemBoxSlots.clear();

	const Vec2 SLOT_SIZE(30.f, 30.f);
	const Vec2 SLOT_SPACING(35.f, 35.f);

	Vec2 startPos = Vec2(200.f, 200.f) - Vec2(2 * SLOT_SPACING.x * 0.5f, 4 * SLOT_SPACING.y * 0.5f);
	startPos += Vec2(SLOT_SPACING.x * 0.5f, SLOT_SPACING.y * 0.5f);

	for (int row = 0; row < 2; ++row) {
		for (int col = 0; col < 4; ++col) {
			int slotIndex = row * 2 + col;

			auto slotObj = make_shared<GameObject>();
			slotObj->SetName(L"ItemBoxSlot_" + to_wstring(slotIndex));

			auto itemSlot = make_shared<ItemSlot>(nullptr, false);
			itemSlot->SetSlotType(SLOTTYPE::INVENTORY);
			slotObj->AddComponent(itemSlot);

			Vec2 slotPos = startPos + Vec2(col * SLOT_SPACING.x, row * SLOT_SPACING.y);
			itemSlot->CreateSlot(slotPos, SLOT_SIZE, slotIndex);

			itemSlot->OnSlotClicked += [this](int _slotIndex, SLOTTYPE _slotType) {
				OnItemBoxSlotClicked(_slotIndex, _slotType);
			};

			m_itemBoxSlots.push_back(itemSlot);
			slotObj->GetTransform()->SetParent(m_itemBox->GetTransform());
		}
	}
	m_itemBox->GetMeshRenderer()->SetActive(false);
	AddUIObject(m_itemBox, true);
	RegisterUIParent(m_itemBox);
}

void LumiaIsland::CheckPickedItemBox()
{
	if (m_pickedObject != nullptr)
	{
		if (m_pickedObject->GetType() == OBJECTTYPE::ITEMBOX && INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
		{
			SOUND->PlaySound(L"SFX/OpenSound_Tomb_01.wav", 16, 0.5f);
			//아이템 박스(m_pickedObject) 데이터 불러옴 -> m_ItemBox에 표시. 
			//그거 누르면, 사용자 인벤토리에 넣기. 
			m_currentItemBox = m_pickedObject;

			UpdateItemBoxSlots(m_currentItemBox);
			m_itemBox->GetMeshRenderer()->SetActive(true);
			//cout << "아이템박스 클릭됨\n";
		}
		else if(m_pickedObject->GetType() != OBJECTTYPE::ITEMBOX && INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
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

void LumiaIsland::OnItemBoxSlotClicked(int _slotIndex, SLOTTYPE _slotType)
{
	if (m_currentItemBox && _slotIndex >= 0 && _slotIndex < m_itemBoxSlots.size()) {
		auto slot = m_itemBoxSlots[_slotIndex];
		if (slot->GetItem() != nullptr) {
			auto itemBoxComponent = m_currentItemBox->GetComponent<ItemBox>();
			if (itemBoxComponent) {

				auto item = itemBoxComponent->DeleteItem(_slotIndex);
				//기존 ItemBox에서 Item삭제. 

				//InventoryManager::Get
				//플레이어 인벤토리에 아이템 추가.

				//UI 슬롯 업데이트. 
			}
		}
	}
}

void LumiaIsland::UpdateItemBoxSlots(shared_ptr<GameObject> _itemBoxObject)
{
	if (!_itemBoxObject)
		return;

	auto itemBoxComponent = _itemBoxObject->GetComponent<ItemBox>();
	if (!itemBoxComponent)
		return;

	//모든 슬롯을 _itemBoxObject의 것으로 업데이트. 

}



Vec4 LumiaIsland::ColorNormalize(Vec4 input)
{
	return input / 255.f;
}

DWORD __stdcall LumiaIsland::BackgroundLoadingThread(LPVOID _param)
{
	LumiaIsland* scene = static_cast<LumiaIsland*>(_param);

	
	try {
		EnterCriticalSection(&scene->m_loadingCS);
		
		ItemManager::GetInstance()->Initialize();
		RecipeManager::GetInstance()->Initialize();
		scene->m_uiManager->InitializeUI();

		scene->LoadItemBoxImages();
		
		LeaveCriticalSection(&scene->m_loadingCS);

		EnterCriticalSection(&scene->m_mainThreadTasksCS);
		scene->m_mainThreadTasks.push([scene]() {
	
			scene->CreateItemBoxPanel();
			
			scene->m_objectsCreated = true;
		});

		LeaveCriticalSection(&scene->m_mainThreadTasksCS);

		scene->m_loadingComplete = true;
		cout << "Succeed Background UI Loading.\n";
	}
	catch (...) {
		OutputDebugStringA("Failed Background Loading...\n");
	}

	return 0;
}

void LumiaIsland::ProcessMainThreadTasks()
{
	EnterCriticalSection(&m_mainThreadTasksCS);

	while (!m_mainThreadTasks.empty()) {
		auto task = m_mainThreadTasks.front();
		m_mainThreadTasks.pop();
		LeaveCriticalSection(&m_mainThreadTasksCS);

		task();

		EnterCriticalSection(&m_mainThreadTasksCS);
	}


	LeaveCriticalSection(&m_mainThreadTasksCS);
}

void LumiaIsland::CreateDefaultLight()
{
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
		shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Bianca", L"Bianca2/Bianca");
		m2->ReadMaterial(L"Bianca2/Bianca");

		/*shared_ptr<Model> m2 = RESOURCES->GetOrAddModel(L"Nicky", L"Nicky/Nicky");
		m2->ReadMaterial(L"Nicky/Nicky");*/


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


void LumiaIsland::ControlPlayerStatus()
{
	PlayerStatus& playerStatus = m_player->GetStatus();
	if (INPUT->GetButton(KEY_TYPE::KEY_1))
	{
		m_player->SetHitAttack(playerStatus.hitAttack + 1);
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_2))
	{
		m_player->SetDefense(playerStatus.defense + 1);
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_3))
	{
		m_player->SetHitSpeed(playerStatus.hitSpeed + 0.01f);
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_4))
	{
		m_player->SetCooldownReduction(playerStatus.cooldownReduction + 10);
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_5))
	{
		m_player->SetMoveSpeed(playerStatus.moveSpeed + 0.01);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::Z))
	{
		m_player->SetHP(playerStatus.hp -= 10);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::C))
	{
		m_player->SetStamina(playerStatus.stamina -= 10);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::B))
	{
		int qLevel = m_player->GetSkill(0)->GetCurSkillLevel();
		int wLevel = m_player->GetSkill(1)->GetCurSkillLevel();
		int eLevel = m_player->GetSkill(2)->GetCurSkillLevel();
		int rLevel = m_player->GetSkill(3)->GetCurSkillLevel();


		cout << "스킬 레벨 : " << qLevel << " , " << wLevel << " , " << eLevel << " , " << rLevel << endl;
	}
	if (INPUT->GetButtonDown(KEY_TYPE::D))
	{
		InventoryManager::GetInstance()->PushItem(ItemManager::GetInstance()->GetItem(L"피아노선"));
	}
}
