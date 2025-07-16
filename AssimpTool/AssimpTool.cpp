#include "pch.h"
#include "AssimpTool.h"
#include "Converter.h"

void AssimpTool::Init()
{
	
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
	//}
	//

	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	converter->ReadAssetFile(L"Aya/Aya_Run.fbx");
	//	converter->ExportAnimationData(L"Aya/Aya_Run");

	//}

	//{

	//	shared_ptr<Converter> converter = make_shared<Converter>();

	//	//FBX -> Memory
	//	converter->ReadAssetFile(L"Bianca/Nicky.fbx");

	//	//Memory ->CustomData;
	//	converter->ExportMaterialData(L"Bianca/Nicky");
	//	converter->ExportModelData(L"Nicky/Nicky");

	//	//CustomData -> Memory;

	//}


	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"Bianca/Bianca_wait.fbx");
		converter->ExportAnimationData(L"Bianca2/Bianca_wait");

	}
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

	}
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
