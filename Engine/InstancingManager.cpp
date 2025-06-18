#include "pch.h"
#include "InstancingManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Transform.h"
#include "Camera.h"

class GameObject;

//�� ���� ������Ʈ�� �߿��� ���������� �ν��Ͻ� �Ǿ�� �ϴ� �κи� ���⼭. 
void InstancingManager::Render(vector<shared_ptr<GameObject>>& _gameObjects)
{
	ClearData();

	RenderMeshRenderer(_gameObjects);
	RenderModelRenderer(_gameObjects);
	RenderAnimRenderer(_gameObjects);
}

void InstancingManager::ClearData()
{
	for (auto& pair : m_buffers) {
		pair.second->ClearData();
	}

}

void InstancingManager::RenderMeshRenderer(vector<shared_ptr<GameObject>>& _gameObjects)
{

	//�ΰ��ӿ� ���� ��� ���̵��� �˻�. 
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;


	//�з� �ܰ�
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		//�� �Ž��� ���� ������ ���� ������� ID���� ������
		//MESH�� MATERIAL 2���� ������� ID��. 
		const InstanceID instanceID = gameObject->GetMeshRenderer()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	//�� �з��� ������ ���� ��ü����. 
	for (auto& pair : cache) {
		const vector<shared_ptr<GameObject>>& vec = pair.second;

		{
			const InstanceID instanceID = pair.first;

			
			for (int32 idx = 0; idx < vec.size(); ++idx) {
				const shared_ptr<GameObject>& gameObject = vec[idx];
				InstancingData data;
				data.m_world = gameObject->GetTransform()->GetWorldMatrix();

				AddData(instanceID, data);
			}

			//���� �׷��ֱ�. 
			shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

			//ù ���� ������Ʈ����, �갡 �׸����� �� ó����Ű��. 
			vec[0]->GetMeshRenderer()->RenderInstancing(buffer);
		}
	}
}

void InstancingManager::RenderModelRenderer(vector<shared_ptr<GameObject>>& _gameObjects)
{
	//�ΰ��ӿ� ���� ��� ���̵��� �˻�. 
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;


	//�з� �ܰ�
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetModelRenderer() == nullptr)
			continue;

		//�� �Ž��� ���� ������ ���� ������� ID���� ������
		//MESH�� MATERIAL 2���� ������� ID��. 
		const InstanceID instanceID = gameObject->GetModelRenderer()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	//�� �з��� ������ ���� ��ü����. 
	for (auto& pair : cache) {
		const vector<shared_ptr<GameObject>>& vec = pair.second;

		{
			const InstanceID instanceID = pair.first;


			for (int32 idx = 0; idx < vec.size(); ++idx) {
				const shared_ptr<GameObject>& gameObject = vec[idx];
				InstancingData data;
				data.m_world = gameObject->GetTransform()->GetWorldMatrix();

				AddData(instanceID, data);
			}

			//���� �׷��ֱ�. 
			shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

			//ù ���� ������Ʈ����, �갡 �׸����� �� ó����Ű��. 
			vec[0]->GetModelRenderer()->RenderInstancing(buffer);
		}
	}
}

void InstancingManager::RenderAnimRenderer(vector<shared_ptr<GameObject>>& _gameObjects)
{

	//�ΰ��ӿ� ���� ��� ���̵��� �˻�. 
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;


	//�з� �ܰ�
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetModelAnimator() == nullptr)
			continue;

		//�� �Ž��� ���� ������ ���� ������� ID���� ������
		//MESH�� MATERIAL 2���� ������� ID��. 
		const InstanceID instanceID = gameObject->GetModelAnimator()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	//�� �з��� ������ ���� ��ü����. 
	for (auto& pair : cache) {
		const vector<shared_ptr<GameObject>>& vec = pair.second;
		shared_ptr<InstancedTweenDesc> tweenDesc = make_shared<InstancedTweenDesc>();

		{
			const InstanceID instanceID = pair.first;


			for (int32 idx = 0; idx < vec.size(); ++idx) {
				const shared_ptr<GameObject>& gameObject = vec[idx];
				InstancingData data;
				data.m_world = gameObject->GetTransform()->GetWorldMatrix();

				AddData(instanceID, data);

				//INSTANCING TWEEN
				gameObject->GetModelAnimator()->UpdateTweenData();
				tweenDesc->tweens[idx] = gameObject->GetModelAnimator()->GetTweenDesc();
				
			}

			//RENDER->PushTweenData(*tweenDesc.get());
			vec[0]->GetModelAnimator()->GetShader()->PushTweenData(*tweenDesc.get());

			//���� �׷��ֱ�. 
			shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

			//ù ���� ������Ʈ����, �갡 �׸����� �� ó����Ű��. 
			vec[0]->GetModelAnimator()->RenderInstancing(buffer);
		}
	}
}

void InstancingManager::AddData(InstanceID _instanceID, InstancingData& _data)
{
	if (m_buffers.find(_instanceID) == m_buffers.end()) {
		m_buffers[_instanceID] = make_shared<InstancingBuffer>();
	}

	m_buffers[_instanceID]->AddData(_data);
}
