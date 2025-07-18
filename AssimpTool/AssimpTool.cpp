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
		converter->ReadAssetFile(L"Map2/Cemetery_STR_Base.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_Base");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_Base");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Pipe_04_Valve.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Pipe_04_Valve");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Pipe_04_Valve");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Alley_OBJ_Fence_04_L.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Alley_OBJ_Fence_04_L");
		converter->ExportModelData(L"Cemetery/Alley_OBJ_Fence_04_L");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Alley_OBJ_Fence_04_M.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Alley_OBJ_Fence_04_M");
		converter->ExportModelData(L"Cemetery/Alley_OBJ_Fence_04_M");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Alley_OBJ_ManHole_03.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Alley_OBJ_ManHole_03");
		converter->ExportModelData(L"Cemetery/Alley_OBJ_ManHole_03");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Alley_OBJ_Fence_04_Pole_S.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Alley_OBJ_Fence_04_Pole_S");
		converter->ExportModelData(L"Cemetery/Alley_OBJ_Fence_04_Pole_S");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Alley_OBJ_Fence_04_S.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Alley_OBJ_Fence_04_S");
		converter->ExportModelData(L"Cemetery/Alley_OBJ_Fence_04_S");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Bg_Cemetery_STR_Base_Wall_01.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Bg_Cemetery_STR_Base_Wall_01");
		converter->ExportModelData(L"Cemetery/Bg_Cemetery_STR_Base_Wall_01");

		//CustomData -> Memory;
	}


	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tomb_02.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tomb_02");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tomb_02");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tomb_03.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tomb_03");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tomb_03");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tombstone_08.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tombstone_08");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tombstone_08");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tomb_04.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tomb_04");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tomb_04");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tombstone_01.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tombstone_01");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tombstone_01");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tombstone_07.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tombstone_07");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tombstone_07");

		//CustomData -> Memory;
	}


	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tombstone_02.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tombstone_02");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tombstone_02");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tombstone_03.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tombstone_03");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tombstone_03");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tombstone_04.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tombstone_04");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tombstone_04");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_OBJ_Tombstone_06.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_OBJ_Tombstone_06");
		converter->ExportModelData(L"Cemetery/Cemetery_OBJ_Tombstone_06");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_GraveBase_03_Wall_01_Fence.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_01_Fence");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_01_Fence");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_House_01.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_House_01");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_House_01");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_BrickDecoTop_03_Broken_01.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_BrickDecoTop_03_Broken_01");
		converter->ExportModelData(L"Cemetery/Common_OBJ_BrickDecoTop_03_Broken_01");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_OuterWall_02_Fence.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_OuterWall_02_Fence");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_OuterWall_02_Fence");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_GraveBase_03_Wall_01.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_01");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_01");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_InnerWall_04.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_InnerWall_04");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_InnerWall_04");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_OuterWall_03_Fence.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_OuterWall_03_Fence");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_OuterWall_03_Fence");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_OuterWall_04_Fence.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_OuterWall_04_Fence");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_OuterWall_04_Fence");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_OuterWall_04.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_OuterWall_04");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_OuterWall_04");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_OuterWall_02.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_OuterWall_02");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_OuterWall_02");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_InnerWall_02.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_InnerWall_02");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_InnerWall_02");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_OuterWall_03.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_OuterWall_03");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_OuterWall_03");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Bg_Cemetery_STR_TextDeco_Set.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Bg_Cemetery_STR_TextDeco_Set");
		converter->ExportModelData(L"Cemetery/Bg_Cemetery_STR_TextDeco_Set");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_GraveBase_03_Wall_02.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_02");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_GraveBase_03_Wall_02");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_GraveBase_02.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_GraveBase_02");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_GraveBase_02");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_InnerWall_05.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_InnerWall_05");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_InnerWall_05");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_House_01_Wall.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_House_01_Wall");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_House_01_Wall");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_House_01_Interior.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_House_01_Interior");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_House_01_Interior");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_OuterWall_02_Grass.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_OuterWall_02_Grass");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_OuterWall_02_Grass");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Cemetery_STR_House_01_Wall_Grass.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Cemetery_STR_House_01_Wall_Grass");
		converter->ExportModelData(L"Cemetery/Cemetery_STR_House_01_Wall_Grass");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Poster_03.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Poster_03");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Poster_03");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Poster_05.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Poster_05");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Poster_05");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Poster_06.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Poster_06");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Poster_06");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Poster_08.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Poster_08");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Poster_08");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Poster_09.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Poster_09");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Poster_09");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Poster_11.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Poster_11");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Poster_11");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Poster_04.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Poster_04");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Poster_04");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Poster_07.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Poster_07");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Poster_07");

		//CustomData -> Memory;
	}

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Map2/Common_OBJ_WaterBush_03.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Cemetery/Common_OBJ_WaterBush_03");
	//	converter->ExportModelData(L"Cemetery/Common_OBJ_WaterBush_03");

	//	//CustomData -> Memory;
	//}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Lotus_03.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Lotus_03");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Lotus_03");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_WaterBush_01.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_WaterBush_01");
		converter->ExportModelData(L"Cemetery/Common_OBJ_WaterBush_01");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_WaterBush_02.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_WaterBush_02");
		converter->ExportModelData(L"Cemetery/Common_OBJ_WaterBush_02");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Lotus_01.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Lotus_01");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Lotus_01");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Lotus_02.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Lotus_02");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Lotus_02");

		//CustomData -> Memory;
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		//FBX -> Memory
		converter->ReadAssetFile(L"Map2/Common_OBJ_Fishing_03.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Cemetery/Common_OBJ_Fishing_03");
		converter->ExportModelData(L"Cemetery/Common_OBJ_Fishing_03");

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
