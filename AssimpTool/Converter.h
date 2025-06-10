#pragma once

#include "AsTypes.h"

class Converter
{
public:
	Converter();
	~Converter();

	void ReadAssetFile(wstring _file);
	void ExportModelData(wstring _savePath);
	void ExportMaterialData(wstring _savePath);


private:
	void ReadModelData(aiNode* _node, int32 _index, int32 _parent);
	void ReadMeshData(aiNode* _node, int32 _bone);
	void WriteModelFile(wstring _filePath);

	void ReadMaterialData();
	void WriteMaterialData(wstring _filePath);
	string WriteTexture(string _saveFolder, string _file);


private:
	const wstring m_assetPath = L"../Resources/Assets/";
	const wstring m_modelPath = L"../Resources/Models/";
	const wstring m_texturePath = L"../Resources/Textures/";

private:
	shared_ptr<Assimp::Importer> m_importer;

	const aiScene* m_scene;

private:
	vector<shared_ptr<asBone>> _bones;
	vector<shared_ptr<asMesh>> _meshes;
	vector<shared_ptr<asMaterial>> _materials;
};



