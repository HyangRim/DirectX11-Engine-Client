#include "pch.h"
#include "AssimpTool.h"
#include "Converter.h"

void AssimpTool::Init()
{
	wstring name[14] = {
		L"Nicky/Nicky_Glove_Atk_01",
		L"Nicky/Nicky_Glove_Atk_02",
		L"Nicky/Nicky_Glove_Run",
		L"Nicky/Nicky_Glove_Skill_01_Attack",
		L"Nicky/Nicky_Glove_Skill_01_End",
		L"Nicky/Nicky_Glove_Skill_01_Rush",
		L"Nicky/Nicky_Glove_Skill_02_Guard",
		L"Nicky/Nicky_Glove_Skill_02_Loop",
		L"Nicky/Nicky_Glove_Skill_03",
		L"Nicky/Nicky_Glove_Skill_04_Attack",
		L"Nicky/Nicky_Glove_Skill_04_Ready",
		L"Nicky/Nicky_Glove_Skill_04_Start",
		L"Nicky/Nicky_Glove_Wait",
	};
	wstring name2[14] = {
		L"Nicky/Nicky_Glove_Atk_01.fbx",
		L"Nicky/Nicky_Glove_Atk_02.fbx",
		L"Nicky/Nicky_Glove_Run.fbx",
		L"Nicky/Nicky_Glove_Skill_01_Attack.fbx",
		L"Nicky/Nicky_Glove_Skill_01_End.fbx",
		L"Nicky/Nicky_Glove_Skill_01_Rush.fbx",
		L"Nicky/Nicky_Glove_Skill_02_Guard.fbx",
		L"Nicky/Nicky_Glove_Skill_02_Loop.fbx",
		L"Nicky/Nicky_Glove_Skill_03.fbx",
		L"Nicky/Nicky_Glove_Skill_04_Attack.fbx",
		L"Nicky/Nicky_Glove_Skill_04_Ready.fbx",
		L"Nicky/Nicky_Glove_Skill_04_Start.fbx",
		L"Nicky/Nicky_Glove_Wait.fbx",
	};


	//{
	//	
	//	shared_ptr<Converter> converter = make_shared<Converter>();

	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Aya/Aya_S005.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Aya/Aya");
	//	converter->ExportModelData(L"Aya/Aya");

	//	//CustomData -> Memory;
	//	

	//	for (int i = 0; i < 14; i++)
	//	{
	//		shared_ptr<Converter> converter = make_shared<Converter>();

	//		//FBX -> Memory
	//		converter->ReadAssetFile(name2[i]);

	//		//Memory ->CustomData;
	//		converter->ExportAnimationData(name[i]);
	//	}
	//}
	

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	converter->ReadAssetFile(L"Aya/Aya_Run.fbx");
	//	converter->ExportAnimationData(L"Aya/Aya_Run");

	//}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/church.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"church/church");
		converter->ExportModelData(L"church/church");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Factory.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Factory/Factory");
		converter->ExportModelData(L"Factory/Factory");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/gojuga.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"gojuga/gojuga");
		converter->ExportModelData(L"gojuga/gojuga");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Grass_Cemetry.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Grass_Cemetry/Grass_Cemetry");
		converter->ExportModelData(L"Grass_Cemetry/Grass_Cemetry");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Grass_Church.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Grass_Church/Grass_Church");
		converter->ExportModelData(L"Grass_Church/Grass_Church");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Grass_Factory.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Grass_Factory/Grass_Factory");
		converter->ExportModelData(L"Grass_Factory/Grass_Factory");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Grass_gojuga.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Grass_gojuga/Grass_gojuga");
		converter->ExportModelData(L"Grass_gojuga/Grass_gojuga");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Grass_Harbor.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Grass_Harbor/Grass_Harbor");
		converter->ExportModelData(L"Grass_Harbor/Grass_Harbor");

		//CustomData -> Memory;
	}


	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Harbor.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Harbor/Harbor");
		converter->ExportModelData(L"Harbor/Harbor");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/hospital.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"hospital/hospital");
		converter->ExportModelData(L"hospital/hospital");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Poles_Factory.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Poles_Factory/Poles_Factory");
		converter->ExportModelData(L"Poles_Factory/Poles_Factory");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Poles_gojuga.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Poles_gojuga/Poles_gojuga");
		converter->ExportModelData(L"Poles_gojuga/Poles_gojuga");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Poles_Harbor.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Poles_Harbor/Poles_Harbor");
		converter->ExportModelData(L"Poles_Harbor/Poles_Harbor");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Rocks_Church.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Rocks_Church/Rocks_Church");
		converter->ExportModelData(L"Rocks_Church/Rocks_Church");

		//CustomData -> Memory;
	}


	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Rocks_Factory.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Rocks_Factory/Rocks_Factory");
		converter->ExportModelData(L"Rocks_Factory/Rocks_Factory");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Rocks_gojuga.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Rocks_gojuga/Rocks_gojuga");
		converter->ExportModelData(L"Rocks_gojuga/Rocks_gojuga");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Rocks_Harbor.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Rocks_Harbor/Rocks_Harbor");
		converter->ExportModelData(L"Rocks_Harbor/Rocks_Harbor");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Rocks_SandBeach.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Rocks_SandBeach/Rocks_SandBeach");
		converter->ExportModelData(L"Rocks_SandBeach/Rocks_SandBeach");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/sand_beach.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"sand_beach/sand_beach");
		converter->ExportModelData(L"sand_beach/sand_beach");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Tree_Cemetery.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Tree_Cemetery/Tree_Cemetery");
		converter->ExportModelData(L"Tree_Cemetery/Tree_Cemetery");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Tree_Church.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Tree_Church/Tree_Church");
		converter->ExportModelData(L"Tree_Church/Tree_Church");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Tree_gojuga.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Tree_gojuga/Tree_gojuga");
		converter->ExportModelData(L"Tree_gojuga/Tree_gojuga");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Tree_Harbor.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Tree_Harbor/Tree_Harbor");
		converter->ExportModelData(L"Tree_Harbor/Tree_Harbor");

		//CustomData -> Memory;
	}
	/*
	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"Nicky/Nicky_Wait.fbx");
		converter->ExportAnimationData(L"Nicky/Nicky_Wait");

	}
	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"Nicky/Nicky_Glove_Run.fbx");
		converter->ExportAnimationData(L"Nicky/Nicky_Glove_Run");

	}
	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"Nicky/Nicky_Glove_Atk1.fbx");
		converter->ExportAnimationData(L"Nicky/Nicky_Glove_Atk1");

	}
	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"Nicky/Nicky_Glove_Atk2.fbx");
		converter->ExportAnimationData(L"Nicky/Nicky_Glove_Atk2");

	}*/
	{
		//shared_ptr<Converter> converter = make_shared<Converter>();
		//converter->ReadAssetFile(L"Bianca/Bianca_atk.fbx");
		//converter->ExportAnimationData(L"Bianca2/Bianca_atk");

	}

	/*
	{

		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"Kachujin/Run.fbx");
		converter->ExportAnimationData(L"Kachujin/Run");

	}

	{

		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"Kachujin/Slash.fbx");
		converter->ExportAnimationData(L"Kachujin/Slash");

	}
	*/
}

void AssimpTool::Update()
{

}

void AssimpTool::Render()
{

}
