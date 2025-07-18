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


	{
		
		shared_ptr<Converter> converter = make_shared<Converter>();

		//FBX -> Memory
		converter->ReadAssetFile(L"Nicky/Nicky_Glove_Skill_01_Charge_Start_Run.fbx");
		converter->ExportAnimationData(L"Nicky/Nicky_Glove_Skill_01_Charge_Start_Run");
		//Memory ->CustomData;
		//converter->ExportMaterialData(L"Aya/Aya");
		//converter->ExportModelData(L"Aya/Aya");

		//CustomData -> Memory;
		

		//for (int i = 0; i < 14; i++)
		//{
		//	shared_ptr<Converter> converter = make_shared<Converter>();

		//	//FBX -> Memory
		//	converter->ReadAssetFile(name2[i]);

		//	//Memory ->CustomData;
		//	converter->ExportAnimationData(name[i]);
		//}
	}
	

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	converter->ReadAssetFile(L"Aya/Aya_Run.fbx");
	//	converter->ExportAnimationData(L"Aya/Aya_Run");

	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/Vegetation_Shurb_01A.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/Vegetation_Shurb_01A");
	//	converter->ExportModelData(L"Environment/Vegetation_Shurb_01A");

	//	//CustomData -> Memory;
	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/Common_OBJ_Lavender_01.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/Common_OBJ_Lavender_01");
	//	converter->ExportModelData(L"Environment/Common_OBJ_Lavender_01");

	//	//CustomData -> Memory;
	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/Common_OBJ_Longgrass_01.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/Common_OBJ_Longgrass_01");
	//	converter->ExportModelData(L"Environment/Common_OBJ_Longgrass_01");

	//	//CustomData -> Memory;
	//}
	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/Woodland_GrassPatch_01.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/Woodland_GrassPatch_01");
	//	converter->ExportModelData(L"Environment/Woodland_GrassPatch_01");

	//	//CustomData -> Memory;
	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/NATURE_FERN_00.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/NATURE_FERN_00");
	//	converter->ExportModelData(L"Environment/NATURE_FERN_00");

	//	//CustomData -> Memory;
	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/NATURE_FERN_04.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/NATURE_FERN_04");
	//	converter->ExportModelData(L"Environment/NATURE_FERN_04");

	//	//CustomData -> Memory;
	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/Wetland_GrassPatch_3.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/Wetland_GrassPatch_3");
	//	converter->ExportModelData(L"Environment/Wetland_GrassPatch_3");

	//	//CustomData -> Memory;
	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/Wetland_GrassPatch_2.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/Wetland_GrassPatch_2");
	//	converter->ExportModelData(L"Environment/Wetland_GrassPatch_2");

	//	//CustomData -> Memory;
	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/Water_Grass_01.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/Water_Grass_01");
	//	converter->ExportModelData(L"Environment/Water_Grass_01");

	//	//CustomData -> Memory;
	//}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/Water_Grass_03_1.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/Water_Grass_03_1");
	//	converter->ExportModelData(L"Environment/Water_Grass_03_1");

	//	//CustomData -> Memory;
	//}



	
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
