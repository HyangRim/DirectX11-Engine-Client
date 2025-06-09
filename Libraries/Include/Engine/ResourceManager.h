#pragma once

#include "ResourceBase.h"

class Shader;
class Texture;
class Mesh;

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager);
public:
	void Init();

	template<typename T>
	shared_ptr<T> Load(const wstring& _key, const wstring& _path);

	template<typename T>
	bool Add(const wstring& _key, shared_ptr<T> _object);

	template<typename T>
	shared_ptr<T> Get(const wstring& _key);

	template<typename T>
	ResourceType GetResourceType();

private:
	void CreateDefaultMesh();

private:
	wstring m_resourcePath;

private:
	using KeyObjMap = map<wstring/*key*/, shared_ptr<ResourceBase>>;
	array<KeyObjMap, RESOURCE_TYPE_COUNT> m_resources;
};

template<typename T>
shared_ptr<T>
ResourceManager::Load(const wstring& _key, const wstring& _path)
{
	auto objectType = GetResourceType<T>();
	KeyObjMap& keyObjMap = m_resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(_key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	shared_ptr<T> object = make_shared<T>();
	object->Load(_path);
	keyObjMap[_key] = object;

	return object;
}

template<typename T>
bool ResourceManager::Add(const wstring& _key, shared_ptr<T> _object)
{
	ResourceType resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = m_resources[static_cast<uint8>(resourceType)];

	auto findIt = keyObjMap.find(_key);
	if (findIt != keyObjMap.end())
		return false;

	keyObjMap[_key] = _object;
	return true;
}

template<typename T>
shared_ptr<T> ResourceManager::Get(const wstring& _key)
{
	ResourceType resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = m_resources[static_cast<uint8>(resourceType)];

	auto findIt = keyObjMap.find(_key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}

template<typename T>
ResourceType ResourceManager::GetResourceType()
{
	if (std::is_same_v<T, Texture>)
		return ResourceType::Texture;
	if (std::is_same_v<T, Mesh>)
		return ResourceType::Mesh;

	assert(false);
	return ResourceType::None;
}

