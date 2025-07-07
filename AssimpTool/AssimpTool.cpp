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

	{

		shared_ptr<Converter> converter = make_shared<Converter>();

		//FBX -> Memory
		converter->ReadAssetFile(L"Bianca/Bianca_mesh.fbx");

		//Memory ->CustomData;
		converter->ExportMaterialData(L"Bianca2/Bianca");
		converter->ExportModelData(L"Bianca2/Bianca");

		//CustomData -> Memory;

	}


	//{
	//	shared_ptr<Converter> converter = make_shared<Converter>();
	//	converter->ReadAssetFile(L"Bianca/Bianca.fbx");
	//	converter->ExportAnimationData(L"Bianca/Bianca_Run");

	//}
	
	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadAssetFile(L"Bianca/Bianca_atk.fbx");
		converter->ExportAnimationData(L"Bianca2/Bianca_atk");

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
