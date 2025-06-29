#include "pch.h"
#include "RenderManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Transform.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "Billboard.h"
#include "SnowBillboard.h"
class GameObject;

//이 게임 오브젝트들 중에서 실질적으로 인스턴싱 되어야 하는 부분만 여기서. 
void RenderManager::Render(vector<shared_ptr<GameObject>>& _gameObjects, bool _isShadowTech)
{
	ClearData();

	m_isShadowTech = _isShadowTech;

	RenderMeshRenderer(_gameObjects);
	RenderModelRenderer(_gameObjects);
	RenderAnimRenderer(_gameObjects);

	//파티클 시스템 있는 것들 선별. 
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		shared_ptr<ParticleSystem> particle = gameObject->GetFixedComponent<ParticleSystem>(ComponentType::ParticleSystem);

		if (particle != nullptr) {
			particle->Render(m_isShadowTech);
		}
		
		shared_ptr<Billboard> billboard = gameObject->GetFixedComponent<Billboard>(ComponentType::Billboard);
		if (billboard != nullptr)
			billboard->Render(m_isShadowTech);

		shared_ptr<SnowBillboard> snowBillboard = gameObject->GetFixedComponent<SnowBillboard>(ComponentType::SnowBillboard);
		if (snowBillboard != nullptr)
			snowBillboard->Render(m_isShadowTech);
	}
}

void RenderManager::ClearData()
{
	for (auto& pair : m_buffers) {
		pair.second->ClearData();
	}

}

void RenderManager::RenderMeshRenderer(vector<shared_ptr<GameObject>>& _gameObjects)
{

	//인게임에 들어온 모든 아이들을 검사. 
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;


	//분류 단계
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		//그 매쉬에 대한 포인터 값을 기반으로 ID값을 가져옴
		//MESH와 MATERIAL 2개를 기반으로 ID값. 
		const InstanceID instanceID = gameObject->GetMeshRenderer()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	//다 분류가 끝나면 같은 물체별로. 
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

			//이제 그려주기. 
			shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

			//첫 번재 오브젝트한테, 얘가 그리도록 일 처리시키기. 
			vec[0]->GetMeshRenderer()->RenderInstancing(buffer, m_isShadowTech);
		}
	}
}

void RenderManager::RenderModelRenderer(vector<shared_ptr<GameObject>>& _gameObjects)
{
	//인게임에 들어온 모든 아이들을 검사. 
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;


	//분류 단계
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetModelRenderer() == nullptr)
			continue;

		//그 매쉬에 대한 포인터 값을 기반으로 ID값을 가져옴
		//MESH와 MATERIAL 2개를 기반으로 ID값. 
		const InstanceID instanceID = gameObject->GetModelRenderer()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	//다 분류가 끝나면 같은 물체별로. 
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

			//이제 그려주기. 
			shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

			//첫 번재 오브젝트한테, 얘가 그리도록 일 처리시키기. 
			vec[0]->GetModelRenderer()->RenderInstancing(buffer, m_isShadowTech);
		}
	}
}

void RenderManager::RenderAnimRenderer(vector<shared_ptr<GameObject>>& _gameObjects)
{

	//인게임에 들어온 모든 아이들을 검사. 
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;


	//분류 단계
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetModelAnimator() == nullptr)
			continue;

		//그 매쉬에 대한 포인터 값을 기반으로 ID값을 가져옴
		//MESH와 MATERIAL 2개를 기반으로 ID값. 
		const InstanceID instanceID = gameObject->GetModelAnimator()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	//다 분류가 끝나면 같은 물체별로. 
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

			//이제 그려주기. 
			shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

			//첫 번재 오브젝트한테, 얘가 그리도록 일 처리시키기. 
			vec[0]->GetModelAnimator()->RenderInstancing(buffer, m_isShadowTech);
		}
	}
}

void RenderManager::AddData(InstanceID _instanceID, InstancingData& _data)
{
	if (m_buffers.find(_instanceID) == m_buffers.end()) {
		m_buffers[_instanceID] = make_shared<InstancingBuffer>();
	}

	m_buffers[_instanceID]->AddData(_data);
}
