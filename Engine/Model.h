#pragma once

struct ModelBone;
struct ModelMesh;

class Model : public enable_shared_from_this<Model>
{
public:
	Model();
	~Model();

public:
	void ReadMaterial(wstring _filename);
	void ReadModel(wstring _filename);

	uint32 GetMaterialCount() { return static_cast<uint32>(m_materials.size()); }
	vector<shared_ptr<Material>>& GetMaterials() { return m_materials; }
	shared_ptr<Material> GetMaterialByIndex(uint32 _index) { return m_materials[_index]; }
	shared_ptr<Material> GetMaterialByName(const wstring& _name);

	uint32 GetMeshCount() { return static_cast<uint32>(m_meshes.size()); }
	vector<shared_ptr<ModelMesh>>& GetMeshes() { return m_meshes; }
	shared_ptr<ModelMesh> GetMeshByIndex(uint32 _index) { return m_meshes[_index]; }
	shared_ptr<ModelMesh> GetMeshByName(const wstring& _name);

	uint32 GetBoneCount() { return static_cast<uint32>(m_bones.size()); }
	vector<shared_ptr<ModelBone>>& GetBones() { return m_bones; }
	shared_ptr<ModelBone> GetBoneByIndex(uint32 _index) { return (_index < 0 || _index >= m_bones.size() ? nullptr : m_bones[_index]); }
	shared_ptr<ModelBone> GetBoneByName(const wstring& _name);

private:
	void BindCacheInfo();

private:
	wstring _modelPath = L"../Resources/Models/";
	wstring _texturePath = L"../Resources/Textures/";

private:
	shared_ptr<ModelBone> m_root;
	vector<shared_ptr<Material>> m_materials;
	vector<shared_ptr<ModelBone>> m_bones;
	vector<shared_ptr<ModelMesh>> m_meshes;

};

