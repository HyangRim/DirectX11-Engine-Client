#include "pch.h"
#include "Converter.h"
#include <filesystem>
#include "Utils.h"
#include "AsTypes.h"
#include "tinyxml2.h"
#include "FileUtils.h"

Converter::Converter()
{
	m_importer = make_shared<Assimp::Importer>();
}

Converter::~Converter()
{

}

void Converter::ReadAssetFile(wstring _file)
{
	wstring fileStr = m_assetPath + _file;

	auto p = std::filesystem::path(fileStr);

	assert(std::filesystem::exists(p));
	m_scene = m_importer->ReadFile(
		Utils::ToString(fileStr),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	assert(m_scene != nullptr);
}

void Converter::ExportModelData(wstring _savePath)
{
	wstring finalPath = m_modelPath + _savePath + L".mesh";
	ReadModelData(m_scene->mRootNode, -1, -1);
	WriteModelFile(finalPath);
}

void Converter::ExportMaterialData(wstring _savePath)
{
	wstring finalPath = m_texturePath + _savePath + L".xml";
	ReadMaterialData();
	WriteMaterialData(finalPath);
}

void Converter::ReadModelData(aiNode* _node, int32 _index, int32 _parent)
{
	shared_ptr<asBone> bone = make_shared<asBone>();
	bone->m_index = _index;
	bone->m_parent = _parent;
	bone->m_name = _node->mName.C_Str();


	//Relative Transform. 
	Matrix transform(_node->mTransformation[0]);

	//행렬을 뒤집어줘야한다? 
	//Bone에서 transform을 가져오는데, 그게 무엇을 기준으로 하는가? 
	//최종 root을 기준으로 한 게 아닌, 바로 직속 부모를 기준으로 한 transform.

	bone->m_transform = transform.Transpose();

	//Local (Root) Transform으로 변환해줘야함.
	// mat Patent는 무엇을 기준으로 하는 것? root으로 가는 직통 경로 행렬. 
	//따라서 말단 자식 노드도 한 번에 root node행렬로 감. 
	Matrix matParent = Matrix::Identity;
	if (_parent >= 0) {
		matParent = _bones[_parent]->m_transform;
	}
	//한 번만 변환 해주면 root node 기준. 
	bone->m_transform = bone->m_transform * matParent;
	_bones.push_back(bone);


	//여기까지 하나의 노드에 대한 거 읽어옴.
	// Mesh 
	ReadMeshData(_node, _index);

	for (uint32 idx = 0; idx < _node->mNumChildren; ++idx) {
		ReadModelData(_node->mChildren[idx], _bones.size(), _index);
	}
}

void Converter::ReadMeshData(aiNode* _node, int32 _bone)
{
	if (_node->mNumMeshes < 1)
		return;

	shared_ptr<asMesh> mesh = make_shared<asMesh>();
	mesh->m_name = _node->mName.C_Str();
	mesh->m_boneIndex = _bone;

	for (uint32 idx = 0; idx < _node->mNumMeshes; ++idx) {
		//Scene에서 들고있던 mesh들 접근. 
		uint32 index = _node->mMeshes[idx];
		const aiMesh* srcMesh = m_scene->mMeshes[index];

		//Material Name
		//매쉬마다 머테리얼이 달라질 수 있음. 그래서 제각기 불러오기. 
		const aiMaterial* material = m_scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->m_materialName = material->GetName().C_Str();

		const uint32 startVertex = mesh->m_vertices.size();

		for (uint32 v = 0; v < srcMesh->mNumVertices; ++v) {

			// Vertex
			VertexType vertex;
			::memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(Vec3));

			// UV
			if (srcMesh->HasTextureCoords(0))
				::memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Vec2));

			// Normal
			if (srcMesh->HasNormals())
				::memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Vec3));

			mesh->m_vertices.push_back(vertex);
		}
		
		//submesh마다 index번호가 같아서는 안됨
		//따라서 ex로 0 ~ 1104, 1105 ~ 3235. 이런 느낌으로 vertices번호가 겹치지 않게끔. 
		for (uint32 f = 0; f < srcMesh->mNumFaces; ++f)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < face.mNumIndices; ++k)
				mesh->indices.push_back(face.mIndices[k] + startVertex);
		}
	}
	_meshes.push_back(mesh);
}

void Converter::WriteModelFile(wstring _filePath)
{

	//FBX기반 사용자 기반 파싱. 
}

void Converter::ReadMaterialData()
{
	for (uint32 idx = 0; idx < m_scene->mNumMaterials; ++idx) {
		aiMaterial* srcMaterial = m_scene->mMaterials[idx];

		shared_ptr<asMaterial> material = make_shared<asMaterial>();

		material->m_name = srcMaterial->GetName().C_Str();

		aiColor3D color;
		// Ambient
		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->m_ambient = Color(color.r, color.g, color.b, 1.f);

		// Diffuse
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->m_diffuse = Color(color.r, color.g, color.b, 1.f);

		// Specular
		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->m_specular = Color(color.r, color.g, color.b, 1.f);
		srcMaterial->Get(AI_MATKEY_SHININESS, material->m_specular.w);

		// Emissive
		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->m_emissive = Color(color.r, color.g, color.b, 1.0f);

		aiString file;

		// Diffuse Texture
		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->m_diffuseFile = file.C_Str();

		// Specular Texture
		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->m_specularFile = file.C_Str();

		// Normal Texture
		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->m_normalFile = file.C_Str();

		_materials.push_back(material);
	}
}

void Converter::WriteMaterialData(wstring _filePath)
{
	auto path = filesystem::path(_filePath);

	filesystem::create_directory(path.parent_path());

	string folder = path.parent_path().string();

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	for (shared_ptr<asMaterial> material : _materials)
	{
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		element = document->NewElement("Name");
		element->SetText(material->m_name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->m_diffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->m_specularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->m_normalFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->m_ambient.x);
		element->SetAttribute("G", material->m_ambient.y);
		element->SetAttribute("B", material->m_ambient.z);
		element->SetAttribute("A", material->m_ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->m_diffuse.x);
		element->SetAttribute("G", material->m_diffuse.y);
		element->SetAttribute("B", material->m_diffuse.z);
		element->SetAttribute("A", material->m_diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->m_specular.x);
		element->SetAttribute("G", material->m_specular.y);
		element->SetAttribute("B", material->m_specular.z);
		element->SetAttribute("A", material->m_specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->m_emissive.x);
		element->SetAttribute("G", material->m_emissive.y);
		element->SetAttribute("B", material->m_emissive.z);
		element->SetAttribute("A", material->m_emissive.w);
		node->LinkEndChild(element);
	}

	document->SaveFile(Utils::ToString(_filePath).c_str());

}

string Converter::WriteTexture(string _saveFolder, string _file)
{
	string fileName = filesystem::path(_file).filename().string();
	string folderName = filesystem::path(_saveFolder).filename().string();

	//경우에 따라 실제로 fbx파일에 텍스처가 포함된 경우도 있음. 
	//그런 경우에는. 
	const aiTexture* srcTexture = m_scene->GetEmbeddedTexture(_file.c_str());
	if (srcTexture)
	{
		string pathStr = _saveFolder + fileName;

		
		if (srcTexture->mHeight == 0)//바이너리 모드. 
		{
			shared_ptr<FileUtils> file = make_shared<FileUtils>();
			file->Open(Utils::ToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);
		}
		else
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = srcTexture->mWidth;
			desc.Height = srcTexture->mHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_IMMUTABLE;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = srcTexture->pcData;

			ComPtr<ID3D11Texture2D> texture;
			HRESULT hr = DEVICE->CreateTexture2D(&desc, &subResource, texture.GetAddressOf());
			CHECK(hr);

			DirectX::ScratchImage img;
			::CaptureTexture(DEVICE.Get(), DC.Get(), texture.Get(), img);

			// Save To File
			hr = DirectX::SaveToDDSFile(*img.GetImages(), DirectX::DDS_FLAGS_NONE, Utils::ToWString(fileName).c_str());
			CHECK(hr);
		}
	}
	else
	{
		// '/'를 쓰면 실제로 이어붙여짐 
		//저걸 스트링으로 추출해서. 
		string originStr = (filesystem::path(m_assetPath) / folderName / _file).string();
		// '/'를 \\로 변경해서 통일. 
		Utils::Replace(originStr, "\\", "/");

		string pathStr = (filesystem::path(_saveFolder) / fileName).string();
		Utils::Replace(pathStr, "\\", "/");
		
		//original Path -> finalPath Copy
		::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
	}

	return fileName;

}
