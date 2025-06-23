#pragma once

#include "InstancingBuffer.h"

class GameObject;

class RenderManager
{
	DECLARE_SINGLE(RenderManager);

public:
	void Render(vector<shared_ptr<GameObject>>& _gameObjects);
	void Clear() { m_buffers.clear(); }
	void ClearData();

private:
	void RenderMeshRenderer(vector<shared_ptr<GameObject>>& _gameObjects);
	void RenderModelRenderer(vector<shared_ptr<GameObject>>& _gameObjects);
	void RenderAnimRenderer(vector<shared_ptr<GameObject>>& _gameObjects);
private:
	void AddData(InstanceID _instanceID, InstancingData& _data);

private:
	map<InstanceID, shared_ptr<InstancingBuffer>> m_buffers;
};

