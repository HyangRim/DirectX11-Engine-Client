#include "pch.h"

#include "LumiaIsland.h"

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

	m_loadingThread = CreateThread(nullptr, 0, BackgroundLoadingThread, this, 0, nullptr);

	
	CreateCemeteryBase();
	CreateCemeteryInterior();
	CreateCemeteryEnvironment();

	cout << "LumiaIsland SelectedCharIndex : " << m_selectedCharacterIdx << endl;
	m_selectedCharacterIdx = 1;
	if (m_selectedCharacterIdx == 0) {
		CreateCharacterBianca();
	}
	else if (m_selectedCharacterIdx == 1) {
		CreateCharacterNicky();
	}

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

	////====================UI====================//
	//LoadItemBoxImages();
	//LoadCharStatIcon();
	//LoadCharEquipmentIcon();
	//LoadCharMainImages();
	//LoadCharInventoryImages();
	//
	//
	/*CreateItemBoxPanel();
	CreateCharStatPanel();
	CreateCharEquipmentPanel();
	CreateCharMainPanel();
	CreateCharInventoryPanel();*/
	////====================UI====================//

	//CreateTestMesh();

	TIME->ResetDeltaTime();
	SOUND->StopAll();
	SOUND->PlayBGM(L"BSER_AreaBGM_CEMETERY.wav", 0.5f);
	Super::Start();
}

void LumiaIsland::Update()
{
	ProcessMainThreadTasks();

	Super::Update();

	if (m_objectsCreated) {
		CheckPickedItemBox();
		UpdateSkillCoolDown();
		ControlPlayerStatus();
		UpdatePlayerStatus();
		UpdateHPAndSPBar();
		UpdatePlayerLevel();
		UpdateTimeline();
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
	statIconColor.push_back(ColorNormalize(Vec4(209, 120, 66 , 255)));
	statIconColor.push_back(ColorNormalize(Vec4(124, 175, 203, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(171, 162, 118, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(200, 200, 200, 255)));
	statIconColor.push_back(ColorNormalize(Vec4(236, 96 , 113, 255)));
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

	panel->Create(Vec2(274.f, 768 - 57), Vec2(155, 115), Vec4(0.f, 0.f, 0.f,0.7f), nullptr);
	m_charStatPanel->SetLayerIndex(LAYER_UI);

	auto imageUI = m_charStatPanel->GetUIPanel()->AddImageUI(Vec2(0, 0), L"ImageUI");

	PlayerStatus& playerStatus = m_player->GetStatus();
	for (int i = 0; i < charStatIconNames.size(); i++)
	{
		wstring prefixTag = L"Ico_ChaStat_";
		shared_ptr<Material> cloneMaterial_charStatIcon = RESOURCES->Get<Material>(prefixTag + charStatIconNames[i])->Clone();
		imageUI->AddImageLayer(i, Vec2(16 + (i % 2) * 70, 13 + (i / 2) * 28), Vec2(17, 17), cloneMaterial_charStatIcon, 5);
	}

	// 스탯 텍스트 설정 구조체
	struct StatTextConfig {
		int col, row;                    // 그리드 위치 (열, 행)
		function<wstring()> getValue;    // 값 가져오는 함수
		Vec4 color;                      // 텍스트 색상
		wstring name;                    // 텍스트 이름
	};

	// 스탯 텍스트 설정 배열
	vector<StatTextConfig> statConfigs = {
		{0, 0, [&]() { return to_wstring((int)playerStatus.hitAttack); },			ColorNormalize(Vec4(218, 187, 102, 255)), L"AttackPower"},
		{1, 0, [&]() { return to_wstring((int)playerStatus.hitAttack); },			ColorNormalize(Vec4(211, 160, 221, 255)), L"SkillAmpRatio"},
		{0, 1, [&]() { return to_wstring((int)playerStatus.hitAttack); },			ColorNormalize(Vec4(209, 120, 66 , 255)), L"IncreaseBasicAttackDamageRatio"},
		{1, 1, [&]() { return to_wstring((int)playerStatus.defense); },				ColorNormalize(Vec4(124, 175, 203, 255)), L"Defense"},
		{0, 2, [&]() { return to_wstring(playerStatus.hitSpeed); },					ColorNormalize(Vec4(171, 162, 118, 255)), L"AttackSpeedRatio"},
		{1, 2, [&]() { return to_wstring((int)playerStatus.cooldownReduction); },	ColorNormalize(Vec4(200, 200, 200, 255)), L"CooldownReduction"},
		{0, 3, [&]() { return to_wstring((int)playerStatus.hitAttack); },			ColorNormalize(Vec4(236, 96 , 113, 255)), L"CriticalStrikeChance"},
		{1, 3, [&]() { return to_wstring(playerStatus.moveSpeed); },				ColorNormalize(Vec4(200, 200, 200, 255)), L"MoveSpeedRatio"}
	};

	// 스탯 텍스트 생성
	for (const auto& config : statConfigs) {
		panel->AddD2DText(
			Vec2(16 + config.col * 70 + 20, 13 + 28 * config.row),
			config.getValue(),
			17.0f,
			config.color,
			1.0f,
			Vec4(0, 0, 0, 0),
			1.0f,
			config.name,
			TextAlignment::Center
		);
	}
	
	AddUIObject(m_charStatPanel, true);
	RegisterUIParent(m_charStatPanel);
}

void LumiaIsland::LoadCharEquipmentIcon()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	wstring prefixTag = L"Ico_Status_";
	vector<wstring> slotTag = { L"Weapon", L"Armor", L"Head", L"Arm", L"Leg" };
	wstring prefixPath = L"..\\Resources\\Textures\\UI\\CharEquipmentIcon\\";

	for (int i = 0; i < slotTag.size(); i++)
	{
		shared_ptr<Material> charEquipIcon = make_shared<Material>();
		SetupUIMaterial(charEquipIcon);

		wstring tag = prefixTag + slotTag[i];
		wstring path = prefixPath + tag + L".png";
		auto charEquipTexture = RESOURCES->Load<Texture>(tag, path);

		charEquipIcon->SetDiffuseMap(charEquipTexture);
		MaterialDesc& charEquipDesc = charEquipIcon->GetMaterialDesc();
		charEquipDesc.ambient = Vec4(1.f);
		charEquipDesc.diffuse = Vec4(1.f);
		charEquipDesc.specular = Vec4(1.0f);
		RESOURCES->Add(tag, charEquipIcon);
	}
}

void LumiaIsland::CreateCharEquipmentPanel()
{
	m_charEquipPanel = make_shared<GameObject>();
	m_charEquipPanel->SetName(L"CharEquipPanel");

	auto panel = make_shared<UIPanel>();
	m_charEquipPanel->AddComponent(panel);

	panel->Create(Vec2(380.f, 768 - 57), Vec2(38, 115), Vec4(0.f, 0.f, 0.f, 0.7f), nullptr);
	m_charEquipPanel->SetLayerIndex(LAYER_UI);

	AddUIObject(m_charEquipPanel, true);
	RegisterUIParent(m_charEquipPanel);

	CreateEquipmentSlots();
}

void LumiaIsland::LoadCharMainImages()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
	};

	wstring prefixPath = L"..\\Resources\\Textures\\UI\\SkillIcon\\";
	vector<wstring> skillTag = { L"P", L"Q", L"W", L"E", L"R"};
	//니키 스킬 아이콘
	for (int i = 0; i < nickySkillIcons.size(); i++)
	{
		shared_ptr<Material> charSkillIcon = make_shared<Material>();
		SetupUIMaterial(charSkillIcon);

		wstring tag = L"Nicky" + skillTag[i];
		wstring path = prefixPath + nickySkillIcons[i] + L".png";
		auto charSkillIconTexture = RESOURCES->Load<Texture>(tag, path);

		charSkillIcon->SetDiffuseMap(charSkillIconTexture);
		MaterialDesc& charSkillIconDesc = charSkillIcon->GetMaterialDesc();
		charSkillIconDesc.ambient = Vec4(1.f);
		charSkillIconDesc.diffuse = Vec4(1.f);
		charSkillIconDesc.specular = Vec4(1.f);
		RESOURCES->Add(tag, charSkillIcon);
	}


	//비앙카 스킬 아이콘
	for (int i = 0; i < biancaSkillIcons.size(); i++)
	{
		shared_ptr<Material> charSkillIcon = make_shared<Material>();
		SetupUIMaterial(charSkillIcon);

		wstring tag = L"Bianca" + skillTag[i];
		wstring path = prefixPath + biancaSkillIcons[i] + L".png";
		auto charSkillIconTexture = RESOURCES->Load<Texture>(tag, path);

		charSkillIcon->SetDiffuseMap(charSkillIconTexture);
		MaterialDesc& charSkillIconDesc = charSkillIcon->GetMaterialDesc();
		charSkillIconDesc.ambient = Vec4(1.f);
		charSkillIconDesc.diffuse = Vec4(1.f);
		charSkillIconDesc.specular = Vec4(1.f);
		RESOURCES->Add(tag, charSkillIcon);
	}


	//hp 이미지
	shared_ptr<Material> charHpBar = make_shared<Material>();
	SetupUIMaterial(charHpBar);

	wstring tag = L"HPBar_UI";
	wstring path = L"..\\Resources\\Textures\\UI\\status\\" + tag + L".png";
	auto charHpBarTexture = RESOURCES->Load<Texture>(tag, path);

	charHpBar->SetDiffuseMap(charHpBarTexture);
	MaterialDesc& charHpBarDesc = charHpBar->GetMaterialDesc();
	charHpBarDesc.ambient = Vec4(1.f);
	charHpBarDesc.diffuse = Vec4(1.f);
	charHpBarDesc.specular = Vec4(1.f);
	RESOURCES->Add(tag, charHpBar);

	//sp 이미지
	shared_ptr<Material> charSpBar = make_shared<Material>();
	SetupUIMaterial(charSpBar);

	tag = L"SPBar_UI";
	path = L"..\\Resources\\Textures\\UI\\status\\" + tag + L".png";
	auto charSpBarTexture = RESOURCES->Load<Texture>(tag, path);

	charSpBar->SetDiffuseMap(charSpBarTexture);
	MaterialDesc& charSpBarDesc = charSpBar->GetMaterialDesc();
	charSpBarDesc.ambient = Vec4(1.f);
	charSpBarDesc.diffuse = Vec4(1.f);
	charSpBarDesc.specular = Vec4(1.f);
	RESOURCES->Add(tag, charSpBar);
}

void LumiaIsland::CreateCharMainPanel()
{
	m_charMainPanel = make_shared<GameObject>();
	m_charMainPanel->SetName(L"CharMainPanel");

	auto panel = make_shared<UIPanel>();
	m_charMainPanel->AddComponent(panel);

	panel->Create(Vec2(615.f, 768 - 57), Vec2(414, 115), Vec4(1.f, 0.f, 1.f, 0.7f), nullptr);
	m_charMainPanel->SetLayerIndex(LAYER_UI);


	wstring characterTag = L"";
	if (m_selectedCharacterIdx == 0) characterTag = L"Bianca";
	else if (m_selectedCharacterIdx == 1) characterTag = L"Nicky";

	auto imageUI = m_charMainPanel->GetUIPanel()->AddImageUI(Vec2(0, 0), L"ImageUI");

	vector<wstring> skillTag = { L"Q", L"W", L"E", L"R" };
	for (int i = 0; i < 4; i++)
	{
		shared_ptr<Material> cloneMaterial_skillIcon = RESOURCES->Get<Material>(characterTag + skillTag[i])->Clone();
		imageUI->AddImageLayer(i, Vec2(128 + 43 * i, 25), Vec2(35, 38), cloneMaterial_skillIcon, 1);
	}
	
	//Q
	auto textQ = panel->AddD2DText(Vec2(128, 25), L"5", 20.0f,
		Vec4(1, 0, 0, 1), 1.0f, Vec4(0, 0, 0, 0), 1.0f,
		L"QSkillCoolDown", TextAlignment::Center);
	textQ->SetUpdateInterval(1.f);

	//W
	auto textW = panel->AddD2DText(Vec2(128 + 43 * 1, 25), L"4", 20.0f,
		Vec4(1, 0, 0, 1), 1.0f, Vec4(0, 0, 0, 0), 1.0f,
		L"WSkillCoolDown", TextAlignment::Center);
	textW->SetUpdateInterval(1.f);

	//E
	auto textE = panel->AddD2DText(Vec2(128 + 43 * 2, 25), L"3", 20.0f,
		Vec4(1, 0, 0, 1), 1.0f, Vec4(0, 0, 0, 0), 1.0f,
		L"ESkillCoolDown", TextAlignment::Center);
	textE->SetUpdateInterval(1.f);

	//R
	auto textR = panel->AddD2DText(Vec2(128 + 43 * 3, 25), L"2", 20.0f,
		Vec4(1, 0, 0, 1), 1.0f, Vec4(0, 0, 0, 0), 1.0f,
		L"RSkillCoolDown", TextAlignment::Center);
	textR->SetUpdateInterval(1.f);


	//HP바 UI
	auto hpPanel = panel->AddPanel(Vec2(194, 70.f), Vec2(153, 10), nullptr, L"ChildHPPanel");
	hpPanel->AddD2DText(
		Vec2(153, 10) / 2.f,
		L"",
		10.f,
		Vec4(1.f, 1.f, 1.f, 1.f),
		1.f,
		Vec4(0.f),
		0.f,
		L"HPText",
		TextAlignment::Center
	);
	auto hpPanelImageUI = hpPanel->AddImageUI(Vec2(0.f), L"HPPanelImageUI");
	hpPanelImageUI->AddImageLayer(0, Vec2(153, 10) / 2.f, Vec2(153, 10) * (1/RESOLUTION_CONSTANT), RESOURCES->Get<Material>(L"HPBar_UI")->Clone(), 1);

	Vec3 hpPos = hpPanelImageUI->GetGameObject()->GetTransform()->GetPosition();


	//SP바 UI
	auto spPanel = panel->AddPanel(Vec2(194, 85.f), Vec2(153, 10), nullptr, L"ChildSPPanel");
	spPanel->AddD2DText(
		Vec2(153, 10) / 2.f,
		L"",
		10.f,
		Vec4(1.f, 1.f, 1.f, 1.f),
		1.f,
		Vec4(0.f),
		0.f,
		L"SPText",
		TextAlignment::Center
	);
	auto spPanelImageUI = spPanel->AddImageUI(Vec2(0.f), L"SPPanelImageUI");
	spPanelImageUI->AddImageLayer(0, Vec2(153, 10) / 2.f, Vec2(153, 10) * (1 / RESOLUTION_CONSTANT), RESOURCES->Get<Material>(L"SPBar_UI")->Clone(), 1);

	//경험치바 UI
	auto expPanel = panel->AddPanel(Vec2(194, 100.f), Vec2(153, 10), nullptr, L"ChildEXPPanel");
	expPanel->AddD2DText(
		Vec2(153, 10) / 2.f,
		L"",
		10.f,
		Vec4(1.f, 1.f, 1.f, 1.f),
		1.f,
		Vec4(0.f),
		0.f,
		L"EXPText",
		TextAlignment::Center
	);
	auto expPanelImageUI = expPanel->AddImageUI(Vec2(0.f), L"EXPPanelImageUI");
	expPanelImageUI->AddImageLayer(0, Vec2(153, 10) / 2.f, Vec2(153, 10) * (1 / RESOLUTION_CONSTANT), RESOURCES->Get<Material>(L"HPBar_UI")->Clone(), 1);


	//캐릭터 이미지 패널 + 레벨
	//캐릭터 초상화
	shared_ptr<Material> cloneMaterial_charLobbyImage;
	if (m_selectedCharacterIdx == 0) cloneMaterial_charLobbyImage = RESOURCES->Get<Material>(L"CharLobbyBianca");
	else if(m_selectedCharacterIdx == 1) cloneMaterial_charLobbyImage = RESOURCES->Get<Material>(L"CharLobbyNicky");

	auto charImagePanel = panel->AddPanel(Vec2(58.f, 58.f), Vec2(100, 100), cloneMaterial_charLobbyImage, L"CharImagePanel");
	auto charLevelPanel = charImagePanel->AddPanel(Vec2(10.f, 80), Vec2(30.f, 30.f), nullptr, L"LevelPanel");
	Vec3 pos = charLevelPanel->GetGameObject()->GetTransform()->GetPosition();
	charLevelPanel->GetGameObject()->GetTransform()->SetPosition(Vec3(pos.x, pos.y, pos.z - 0.01));


	charLevelPanel->AddD2DText(
		Vec2(15.f, 15.f),
		L"20",
		12.f,
		Vec4(1.f),
		1.f,
		Vec4(0.f),
		0.f,
		L"LevelText",
		TextAlignment::Center
	);

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

	panel->Create(Vec2(960.f, 768 - 57), Vec2(252, 62), Vec4(0.f, 0.f, 0.f, 0.f), nullptr);
	m_charInventoryPanel->SetLayerIndex(LAYER_UI);

	AddUIObject(m_charInventoryPanel, true);
	RegisterUIParent(m_charInventoryPanel);

	CreateInventorySlots();
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

		scene->LoadItemBoxImages();
		scene->LoadCharStatIcon();
		scene->LoadCharEquipmentIcon();
		scene->LoadCharMainImages();
		scene->LoadCharInventoryImages();
		scene->LoadItemIcons();
		scene->LoadItemGradeMaterial();
		scene->LoadCharEquipmentIcon();
		scene->LoadTimeImage();
		
		LeaveCriticalSection(&scene->m_loadingCS);

		EnterCriticalSection(&scene->m_mainThreadTasksCS);
		scene->m_mainThreadTasks.push([scene]() {

			scene->CreateTestItems();

			scene->CreateItemBoxPanel();
			scene->CreateCharStatPanel();
			scene->CreateCharEquipmentPanel();
			scene->CreateCharMainPanel();
			scene->CreateCharInventoryPanel();

			
			scene->CreateInventoryManager();
			scene->CreateTimePanel();
			scene->CreateDayPanel();
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

void LumiaIsland::LoadItemIcons()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
	};

	wstring prefixPath = L"..\\Resources\\Textures\\UI\\ItemIcon\\";
	vector<int> itemTag = { 110406, 110504, 201413, 202409, 203506, 204408 };

	for (int i = 0; i < 6; i++)
	{
		shared_ptr<Material> itemIcon = make_shared<Material>();
		SetupUIMaterial(itemIcon);

		wstring tag = L"ItemIcon_" + to_wstring(itemTag[i]);
		wstring path = prefixPath + tag + L".png";
		auto itemIconTexture = RESOURCES->Load<Texture>(tag, path);

		itemIcon->SetDiffuseMap(itemIconTexture);
		MaterialDesc& itemIconDesc = itemIcon->GetMaterialDesc();
		itemIconDesc.ambient = Vec4(1.f);
		itemIconDesc.diffuse = Vec4(1.f);
		itemIconDesc.specular = Vec4(1.f);
		RESOURCES->Add(tag, itemIcon);
	}
}

void LumiaIsland::LoadItemGradeMaterial()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	wstring prefixPath = L"..\\Resources\\Textures\\UI_Btn\\";
	wstring prefixBtnTag = L"Img_Item_Slot_";
	vector<wstring> gradeTag = { L"Common", L"Uncommon", L"Rare", L"Epic", L"Legendary"};
	
	for (int i = 0; i < 5; i++)
	{
		shared_ptr<Material> btnImg = make_shared<Material>();
		SetupUIMaterial(btnImg);

		wstring tag = prefixBtnTag + gradeTag[i];
		wstring path = prefixPath + tag + L".png";
		auto btnImgTexture = RESOURCES->Load<Texture>(tag, path);

		btnImg->SetDiffuseMap(btnImgTexture);
		MaterialDesc& btnImgDesc = btnImg->GetMaterialDesc();
		btnImgDesc.ambient = Vec4(1.f);
		btnImgDesc.diffuse = Vec4(1.f);
		btnImgDesc.specular = Vec4(1.f);
		RESOURCES->Add(tag, btnImg);
	}
}


void LumiaIsland::UpdateSkillCoolDown()
{
	vector<shared_ptr<D2DText>> skillCoolDownTextUI;
	vector<wstring> skillNames = { L"QSkillCoolDown", L"WSkillCoolDown", L"ESkillCoolDown", L"RSkillCoolDown" };

	for (const auto& skillName : skillNames) {
		skillCoolDownTextUI.push_back(m_charMainPanel->GetUIPanel()->GetD2DText(skillName));
	}

	for (int i = 0; i < 4; i++) {
		ISkill* skill = m_player->GetSkill(i);
		int skillCurCoolDown = (int)(skill->GetCurrentCooldown());

		if (skillCoolDownTextUI[i]) {
			skillCoolDownTextUI[i]->SetText(to_wstring(skillCurCoolDown));
			//skillCoolDownTextUI[i]->SetVisible(skillCurCoolDown > 0); // 0이면 숨김, 아니면 표시
		}
	}
}

void LumiaIsland::UpdatePlayerStatus()
{
	vector<shared_ptr<D2DText>> playerStatusTextUI;
	vector<wstring> statusNames = charStatIconNames;

	for (const auto& statusName : statusNames) {
		playerStatusTextUI.push_back(m_charStatPanel->GetUIPanel()->GetD2DText(statusName));
	}

	PlayerStatus& playerStatus = m_player->GetStatus();

	// 소숫점 1자리로 제한하는 함수
	auto FormatFloat = [](float value, int precision = 1) -> wstring {
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(precision) << value;
		return ss.str();
		};

	playerStatusTextUI[0]->SetText(to_wstring((int)playerStatus.hitAttack));     // 정수
	playerStatusTextUI[1]->SetText(to_wstring((int)playerStatus.hitAttack));     // 정수
	playerStatusTextUI[2]->SetText(to_wstring((int)playerStatus.hitAttack));     // 정수
	playerStatusTextUI[3]->SetText(to_wstring((int)playerStatus.defense));       // 정수
	playerStatusTextUI[4]->SetText(FormatFloat(playerStatus.hitSpeed, 1));       // 소숫점 1자리
	playerStatusTextUI[5]->SetText(to_wstring((int)playerStatus.cooldownReduction)); // 정수
	playerStatusTextUI[6]->SetText(to_wstring((int)playerStatus.hitAttack));     // 정수
	playerStatusTextUI[7]->SetText(FormatFloat(playerStatus.moveSpeed, 1));      // 소숫점 1자리
}

void LumiaIsland::UpdateHPAndSPBar()
{
	PlayerStatus& playerStatus = m_player->GetStatus();

	auto hpPanel = m_charMainPanel->GetUIPanel()->GetChildUIPanel(L"ChildHPPanel");
	auto spPanel = m_charMainPanel->GetUIPanel()->GetChildUIPanel(L"ChildSPPanel");
	auto expPanel = m_charMainPanel->GetUIPanel()->GetChildUIPanel(L"ChildEXPPanel");


	auto hpPanelText = hpPanel->GetD2DText(L"HPText");
	wstring hpText = to_wstring(playerStatus.hp) + L"/" + to_wstring(playerStatus.max_HP);
	hpPanelText->SetText(hpText);

	// 직접 size 수정 대신 SetLayerSize() 사용
	auto hpImageUI = hpPanel->GetImageUI(L"HPPanelImageUI");
	float ratio = ((float)playerStatus.hp / (float)playerStatus.max_HP);
	Vec2 newSize = Vec2(153.f * ratio, 10.f);
	Vec2 newPos = Vec2(153.f /2.f - (153.f/2.f) * (1-ratio), 10.f / 2.f);
	hpImageUI->SetLayerSize(0, newSize);  // 레이어 0의 크기 변경
	hpImageUI->SetLayerPosition(0, newPos);


	auto spPanelText = spPanel->GetD2DText(L"SPText");
	wstring spText = to_wstring(playerStatus.stamina) + L"/" + to_wstring(playerStatus.max_Stamina);
	spPanelText->SetText(spText);

	// 직접 size 수정 대신 SetLayerSize() 사용
	auto spImageUI = spPanel->GetImageUI(L"SPPanelImageUI");
	ratio = ((float)playerStatus.stamina / (float)playerStatus.max_Stamina);
	newSize = Vec2(153.f * ratio, 10.f);
	newPos = Vec2(153.f / 2.f - (153.f / 2.f) * (1 - ratio), 10.f / 2.f);
	spImageUI->SetLayerSize(0, newSize);  // 레이어 0의 크기 변경
	spImageUI->SetLayerPosition(0, newPos);


	auto expPanelText = expPanel->GetD2DText(L"EXPText");
	wstring expText = to_wstring(playerStatus.curExp) + L"/" + to_wstring(playerStatus.curExpLimit);
	expPanelText->SetText(expText);

	// 직접 size 수정 대신 SetLayerSize() 사용
	auto expImageUI = expPanel->GetImageUI(L"EXPPanelImageUI");
	ratio = ((float)playerStatus.curExp / (float)playerStatus.curExpLimit);
	newSize = Vec2(153.f * ratio, 10.f);
	newPos = Vec2(153.f / 2.f - (153.f / 2.f) * (1 - ratio), 10.f / 2.f);
	expImageUI->SetLayerSize(0, newSize);  // 레이어 0의 크기 변경
	expImageUI->SetLayerPosition(0, newPos);
}

void LumiaIsland::UpdatePlayerLevel()
{
	auto levelPanel = m_charMainPanel->GetUIPanel()->GetChildUIPanel(L"CharImagePanel")->GetChildUIPanel(L"LevelPanel");

	PlayerStatus& playerStatus = m_player->GetStatus();

	levelPanel->GetD2DText(L"LevelText")->SetText(to_wstring(playerStatus.level));
}

void LumiaIsland::UpdateTimeline()
{
	if (!m_timePanel) return;

	m_lastFloatTime += DT;

	int currentSeconds = static_cast<int>(m_lastFloatTime);

	if (currentSeconds <= m_lastTime)
		return;

	auto timeText = m_timePanel->GetUIPanel()->GetD2DText(L"TimeText");
	if (timeText) {
		// 현재 게임 시간 계산 (예시)
		float currentTime = m_lastFloatTime; // 또는 게임 시간 로직
		int minutes = (int)(currentTime / 60.0f);
		int seconds = (int)(currentTime) % 60;

		wchar_t timeBuffer[8];
		swprintf_s(timeBuffer, 8, L"%02d : %02d", minutes, seconds);
		wstring timeString = timeBuffer;

		timeText->SetText(timeString);
	}
}

void LumiaIsland::LoadTimeImage()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
	};
	
	wstring prefixPath = L"..\\Resources\\Textures\\UI\\time\\";

	{
		shared_ptr<Material> TimeUIImage = make_shared<Material>();
		SetupUIMaterial(TimeUIImage);

		wstring path = prefixPath + L"Time_UI_Bg.png";
		auto TimeUITexture = RESOURCES->Load<Texture>(L"Time_UI_Image", path);

		TimeUIImage->SetDiffuseMap(TimeUITexture);
		MaterialDesc& TimeUIDesc = TimeUIImage->GetMaterialDesc();
		TimeUIDesc.ambient = Vec4(1.f);
		TimeUIDesc.diffuse = Vec4(1.f);
		TimeUIDesc.specular = Vec4(1.0f);
		RESOURCES->Add(L"Time_UI_BG", TimeUIImage);
	}

	//=====================날짜관련 함수=====================//
	{
		shared_ptr<Material> TimeUIImage = make_shared<Material>();
		SetupUIMaterial(TimeUIImage);

		wstring path = prefixPath + L"Img_HUD_Union.png";
		auto TimeUITexture = RESOURCES->Load<Texture>(L"DAY_UI_Image", path);

		TimeUIImage->SetDiffuseMap(TimeUITexture);
		MaterialDesc& TimeUIDesc = TimeUIImage->GetMaterialDesc();
		TimeUIDesc.ambient = Vec4(1.f);
		TimeUIDesc.diffuse = Vec4(1.f);
		TimeUIDesc.specular = Vec4(1.0f);
		RESOURCES->Add(L"DAY_UI_BG", TimeUIImage);
	}
	//Ico_DaySun.png

	{
		shared_ptr<Material> sunIcon = make_shared<Material>();
		SetupUIMaterial(sunIcon);
		auto sunIconTexture = RESOURCES->Load<Texture>(L"SUN_ICON", prefixPath + L"Ico_DaySun.png"); // 실제 파일명으로 변경
		sunIcon->SetDiffuseMap(sunIconTexture);
		MaterialDesc& sunIconDesc = sunIcon->GetMaterialDesc();
		sunIconDesc.ambient = Vec4(1.f);
		sunIconDesc.diffuse = Vec4(1.f);
		sunIconDesc.specular = Vec4(1.0f);
		RESOURCES->Add(L"SUN_UI_ICON", sunIcon);
	}
}

void LumiaIsland::CreateTimePanel()
{
	m_timePanel = make_shared<GameObject>();
	m_timePanel->SetName(L"Time Panel");

	auto timePanel = make_shared<UIPanel>();
	m_timePanel->AddComponent(timePanel);


	shared_ptr<Material> TimePanelBackGround = RESOURCES->Get<Material>(L"Time_UI_BG")->Clone();
	timePanel->Create(Vec2(GAME->GetGameDesc().width / 2.f , 0.f), Vec2(117, 58), Vec4(1.f, 1.f, 1.f, 0.5f), TimePanelBackGround);
	m_timePanel->SetLayerIndex(LAYER_UI);

	// 가운데에 하얀 텍스트 추가
	timePanel->AddD2DText(
		Vec2(117 / 2.f, 58 / 2.f + 14.f),      // 패널 가운데 위치
		L"00 : 00",                        // 시간 텍스트 (예시)
		16.0f,                          // 폰트 크기
		Vec4(1.f, 1.f, 1.f, 1.f),      // 하얀색 (RGBA)
		1.0f,                           // 불투명도
		Vec4(0, 0, 0, 0),               // 배경색 (투명)
		0.0f,                           // 배경 불투명도
		L"TimeText",                    // 텍스트 이름
		TextAlignment::Center           // 가운데 정렬
	);

	m_timePanel->GetMeshRenderer()->SetActive(true);

	AddUIObject(m_timePanel, true);
	RegisterUIParent(m_timePanel);
}

void LumiaIsland::CreateDayPanel()
{
	m_dayPanel = make_shared<GameObject>();
	m_dayPanel->SetName(L"Day Panel");

	auto dayPanel = make_shared<UIPanel>();
	m_dayPanel->AddComponent(dayPanel);


	shared_ptr<Material> TimePanelBackGround = RESOURCES->Get<Material>(L"Time_UI_BG")->Clone();
	dayPanel->Create(Vec2(GAME->GetGameDesc().width / 2.f - 70.f , 0.f), Vec2(58, 58), Vec4(1.f, 1.f, 1.f, 0.5f), TimePanelBackGround);
	m_dayPanel->SetLayerIndex(LAYER_UI);

	// 태양 아이콘 ImageUI 먼저 추가 (텍스트 뒤에 배치)
	auto sunImageUI = dayPanel->AddImageUI(Vec2(58 / 2.f, 58 / 2.f - 5.f), L"SUN_UI_ICON");
	shared_ptr<Material> sunIconMaterial = RESOURCES->Get<Material>(L"SUN_UI_ICON")->Clone();
	sunImageUI->AddImageLayer(
		5,                              // 레이어 인덱스
		Vec2(0, 0),                     // 로컬 위치 (ImageUI 내에서의 위치)
		Vec2(24, 24),                   // 이미지 크기 (패널에 맞게 조정)
		sunIconMaterial,                // 머티리얼
		1                               // 렌더 순서
	);

	// 가운데에 하얀 텍스트 추가
	dayPanel->AddD2DText(
		Vec2(58 / 2.f , 58 / 2.f + 6.f),      // 패널 가운데 위치
		L"1일 차",                        // 시간 텍스트 (예시)
		8.0f,                          // 폰트 크기
		Vec4(1.f, 1.f, 1.f, 1.f),      // 하얀색 (RGBA)
		1.0f,                           // 불투명도
		Vec4(0, 0, 0, 0),               // 배경색 (투명)
		0.0f,                           // 배경 불투명도
		L"TimeText",                    // 텍스트 이름
		TextAlignment::Center           // 가운데 정렬
	);

	m_dayPanel->GetMeshRenderer()->SetActive(true);

	AddUIObject(m_dayPanel, true);
	RegisterUIParent(m_dayPanel);
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
		m_player->SetLevel(playerStatus.level += 1);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::D))
	{
		m_player->SetCurExp(playerStatus.curExp += 1);
	}
}


void LumiaIsland::CreateInventorySlots()
{
	m_inventorySlots.clear();

	// 5x2 그리드로 10개 슬롯 생성
	int slotsX = 5;
	int slotsY = 2;
	Vec2 slotSize = Vec2(46, 28);
	Vec2 spacing = Vec2(5, 5);
	Vec2 startPos = Vec2(960.f - (252 /2.f)+23, (768 - 57) - (62 /2.f) + 14); // 패널 내 시작 위치
	Vec2 panelSize = Vec2(252, 62);

	for (int row = 0; row < slotsY; row++)
	{
		for (int col = 0; col < slotsX; col++)
		{
			int slotIndex = row * slotsX + col;

			// ItemSlot 생성
			shared_ptr<ItemSlot> itemSlot;
			
			if (row == 0 && col == 0) itemSlot = make_shared<ItemSlot>(m_testItems[0], true);
			else if (row == 0 && col == 1) itemSlot = make_shared<ItemSlot>(m_testItems[1], true);
			else if (row == 0 && col == 2) itemSlot = make_shared<ItemSlot>(m_testItems[2], true);
			else if (row == 0 && col == 3) itemSlot = make_shared<ItemSlot>(m_testItems[3], true);
			else if (row == 0 && col == 4) itemSlot = make_shared<ItemSlot>(m_testItems[4], true);

			else itemSlot = make_shared<ItemSlot>(m_testItems[5], true);

			itemSlot->SetSlotType(SLOTTYPE::INVENTORY);
			Vec2 slotPos = Vec2(
				startPos.x + col * (slotSize.x + spacing.x),
				startPos.y + row * (slotSize.y + spacing.y)
			);
			itemSlot->CreateSlot(slotPos, slotSize, slotIndex);

			m_inventorySlots.push_back(itemSlot);
		}
	}
}
void LumiaIsland::CreateEquipmentSlots()
{
	m_equipmentSlots.clear();

	// 5x2 그리드로 10개 슬롯 생성
	int slotsX = 1;
	int slotsY = 5;
	Vec2 slotSize = Vec2(34, 22);
	Vec2 spacing = Vec2(0, 1);

	Vec2 startPos = Vec2(380.f - (38 / 2.f) + 19, (768 - 57) - (115 / 2.f) + 13); // 패널 내 시작 위치
	Vec2 panelSize = Vec2(38, 115);

	for (int row = 0; row < slotsY; row++)
	{
		for (int col = 0; col < slotsX; col++)
		{
			int slotIndex = row * slotsX + col;

			// ItemSlot 생성
			auto itemSlot = make_shared<ItemSlot>(nullptr, false);
			itemSlot->SetSlotType(SLOTTYPE::EQUIPMENT);
			Vec2 slotPos = Vec2(
				startPos.x + col * (slotSize.x + spacing.x),
				startPos.y + row * (slotSize.y + spacing.y)
			);
			itemSlot->CreateSlot(slotPos, slotSize, slotIndex);

			m_equipmentSlots.push_back(itemSlot);
		}
	}
}
void LumiaIsland::CreateTestItems()
{
	m_testItems.clear();

	// 테스트 아이템 1: 일반 무기
	auto weapon = make_shared<EquipableItem>();
	weapon->SetItemID(110406); // 기존에 로드된 아이템 아이콘 ID
	weapon->SetName(L"IronSword");
	weapon->SetDescription(L"날카로운 철검입니다.");
	weapon->SetItemType(ITEMTYPE::EQUIPABLE);
	weapon->SetItemGrade(ITEMGRADE::COMMON);
	weapon->SetEquipType(EquipmentType::WEAPON);

	ItemStatus weaponStatus;
	weaponStatus.attackPower = 50;
	weaponStatus.attackSpeed = 1.2f;
	weapon->SetStatus(weaponStatus);

	m_testItems.push_back(weapon);

	// 테스트 아이템 2: 희귀 방어구
	auto armor = make_shared<EquipableItem>();
	armor->SetItemID(110504);
	armor->SetName(L"IronPlate");
	armor->SetDescription(L"튼튼한 강철 갑옷입니다.");
	armor->SetItemType(ITEMTYPE::EQUIPABLE);
	armor->SetItemGrade(ITEMGRADE::RARE);
	armor->SetEquipType(EquipmentType::CHEST);

	ItemStatus armorStatus;
	armorStatus.defense = 30;
	armorStatus.maxHP = 100;
	armor->SetStatus(armorStatus);

	m_testItems.push_back(armor);

	// 테스트 아이템 3: 전설 아이템
	auto legendary = make_shared<EquipableItem>();
	legendary->SetItemID(201413);
	legendary->SetName(L"DragonSword");
	legendary->SetDescription(L"전설의 드래곤 검입니다.");
	legendary->SetItemType(ITEMTYPE::EQUIPABLE);
	legendary->SetItemGrade(ITEMGRADE::LEGENDARY);
	legendary->SetEquipType(EquipmentType::HEAD);

	ItemStatus legendaryStatus;
	legendaryStatus.attackPower = 150;
	legendaryStatus.attackSpeed = 1.8f;
	legendaryStatus.lifeSteal = 10.0f;
	legendary->SetStatus(legendaryStatus);

	m_testItems.push_back(legendary);

	// 테스트 아이템 4: 전설 아이템
	auto lsword = make_shared<EquipableItem>();
	lsword->SetItemID(202409);
	lsword->SetName(L"LSword");
	lsword->SetDescription(L"L곤 검입니다.");
	lsword->SetItemType(ITEMTYPE::EQUIPABLE);
	lsword->SetItemGrade(ITEMGRADE::LEGENDARY);
	lsword->SetEquipType(EquipmentType::CHEST);

	ItemStatus lStatus;
	lStatus.attackPower = 150;
	lStatus.attackSpeed = 1.8f;
	lStatus.lifeSteal = 10.0f;
	lsword->SetStatus(lStatus);

	m_testItems.push_back(lsword);

	// 테스트 아이템 5: 전설 아이템
	auto legendary2 = make_shared<EquipableItem>();
	legendary2->SetItemID(203506);
	legendary2->SetName(L"L1Sword");
	legendary2->SetDescription(L"L1곤 검입니다.");
	legendary2->SetItemType(ITEMTYPE::EQUIPABLE);
	legendary2->SetItemGrade(ITEMGRADE::LEGENDARY);
	legendary2->SetEquipType(EquipmentType::ARM);

	ItemStatus legendary2Status;
	legendary2Status.attackPower = 150;
	legendary2Status.attackSpeed = 1.8f;
	legendary2Status.lifeSteal = 10.0f;
	legendary2->SetStatus(legendary2Status);

	m_testItems.push_back(legendary2);

	// 테스트 아이템 6: 전설 아이템
	auto legendary3 = make_shared<EquipableItem>();
	legendary3->SetItemID(204408);
	legendary3->SetName(L"L2Sword");
	legendary3->SetDescription(L"L2곤 검입니다.");
	legendary3->SetItemType(ITEMTYPE::EQUIPABLE);
	legendary3->SetItemGrade(ITEMGRADE::LEGENDARY);
	legendary3->SetEquipType(EquipmentType::LEG);

	ItemStatus legendary3Status;
	legendary3Status.attackPower = 150;
	legendary3Status.attackSpeed = 1.8f;
	legendary3Status.lifeSteal = 10.0f;
	legendary3->SetStatus(legendary3Status);

	m_testItems.push_back(legendary3);

}

void LumiaIsland::CreateInventoryManager()
{
	auto managerObj = make_shared<GameObject>();
	managerObj->SetName(L"InventoryManager");

	m_inventoryManager = make_shared<InventoryManager>();
	managerObj->AddComponent(m_inventoryManager);

	// 슬롯들 등록
	m_inventoryManager->RegisterInventorySlots(m_inventorySlots);
	m_inventoryManager->RegisterEquipmentSlots(m_equipmentSlots);
	m_inventoryManager->SetPlayer(m_player);
}