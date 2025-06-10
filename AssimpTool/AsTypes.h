#pragma once


using VertexType = VertexTextureNormalTangentBlendData;

struct asBone {
	string m_name;

	int32 m_index = -1;
	int32 m_parent = -1;

	Matrix m_transform;
};

struct asMesh
{
	string m_name;
	aiMesh* m_mesh;
	vector<VertexType> m_vertices;
	vector<uint32> indices;

	int32 m_boneIndex;
	string m_materialName;
};

struct asMaterial
{
	string m_name;
	Color m_ambient;
	Color m_diffuse;
	Color m_specular;
	Color m_emissive;

	string m_diffuseFile;
	string m_specularFile;
	string m_normalFile;
};