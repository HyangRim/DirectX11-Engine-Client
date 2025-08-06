#include "pch.h"
#include "AssimpTool.h"
#include "Converter.h"

void AssimpTool::Start()
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
		converter->ReadAssetFile(L"Nicky/NickyESkill_Mesh.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Nicky/NickyESkill_Mesh");
		converter->ExportModelData(L"Nicky/NickyESkill_Mesh");

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
	//	converter->ReadAssetFile(L"Bianca/BiancaCoffin_mesh.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Bianca/BiancaCoffin_mesh");
	//	converter->ExportModelData(L"Bianca/BiancaCoffin_mesh");

	//	//CustomData -> Memory;
	//}
	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Bianca/BiancaUmbrella_mesh.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Bianca/BiancaUmbrella_mesh");
	//	converter->ExportModelData(L"Bianca/BiancaUmbrella_mesh");

	//	//CustomData -> Memory;
	//}
	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	converter->ReadAssetFile(L"Bianca/BiancaUmbrella_anim1.fbx");
	//	converter->ExportAnimationData(L"Bianca/BiancaUmbrella_anim1");
	//}
	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	converter->ReadAssetFile(L"Bianca/BiancaUmbrella_anim2.fbx");
	//	converter->ExportAnimationData(L"Bianca/BiancaUmbrella_anim2");
	//}



	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Environment/CampFire.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Environment/CampFire");
	//	converter->ExportModelData(L"Environment/CampFire");

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
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"wolf/wolf_appearwait_anim.fbx");
		converter->ExportAnimationData(L"wolf/wolf_appearwait_anim");

	}
	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"wolf/wolf_appear_anim.fbx");
		converter->ExportAnimationData(L"wolf/wolf_appear_anim");

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
