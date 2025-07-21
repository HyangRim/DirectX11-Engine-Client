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
#include "Material.h"
#include "Light.h"

class GameObject;

void RenderManager::Init()
{
	// 디퍼드 라이팅 셰이더 생성 및 설정
	auto deferredShader = make_shared<Shader>(L"00. DeferredLighting.fx");
	SetDeferredLightingShader(deferredShader);

	// 디퍼드 렌더링 활성화
	SetDeferredRendering(true);
}

void RenderManager::Render(vector<shared_ptr<GameObject>>& _gameObjects, bool _isShadowTech)
{
	if (m_useDeferredRendering && !_isShadowTech) {
		RenderDeferred(_gameObjects, _isShadowTech);
	}
	else {
		RenderForward(_gameObjects, _isShadowTech);
	}
}

//이 게임 오브젝트들 중에서 실질적으로 인스턴싱 되어야 하는 부분만 여기서. 
void RenderManager::RenderForward(vector<shared_ptr<GameObject>>& _gameObjects, bool _isShadowTech)
{
	ClearData();

	m_isShadowTech = _isShadowTech;

	RenderMeshRendererForward(_gameObjects);
	RenderModelRendererForward(_gameObjects);
	RenderAnimRendererForward(_gameObjects);

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

void RenderManager::RenderDeferred(vector<shared_ptr<GameObject>>& _gameObjects, bool _isShadowTech)
{
	ClearData();
	m_isShadowTech = _isShadowTech;

	// 1단계: G-Buffer 패스
	GRAPHICS->BeginGeometryPass();
	RenderGeometryPass(_gameObjects);

	// 2단계: 디퍼드 라이팅 패스
	GRAPHICS->BeginLightingPass();
	RenderDeferredLighting();

	// 3단계: 투명 객체 (포워드 방식)
	RenderTransparentObjects(_gameObjects);

	// 4단계: 파티클 시스템 등
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		shared_ptr<ParticleSystem> particle = gameObject->GetFixedComponent<ParticleSystem>(ComponentType::ParticleSystem);
		if (particle != nullptr) {
			particle->Render(false);
		}

		shared_ptr<Billboard> billboard = gameObject->GetFixedComponent<Billboard>(ComponentType::Billboard);
		if (billboard != nullptr)
			billboard->Render(false);

		shared_ptr<SnowBillboard> snowBillboard = gameObject->GetFixedComponent<SnowBillboard>(ComponentType::SnowBillboard);
		if (snowBillboard != nullptr)
			snowBillboard->Render(false);
	}
}

void RenderManager::ClearData()
{
	for (auto& pair : m_buffers) {
		pair.second->ClearData();
	}

}

void RenderManager::RenderMeshRendererForward(vector<shared_ptr<GameObject>>& _gameObjects)
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

void RenderManager::RenderModelRendererForward(vector<shared_ptr<GameObject>>& _gameObjects)
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

void RenderManager::RenderAnimRendererForward(vector<shared_ptr<GameObject>>& _gameObjects)
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

void RenderManager::RenderMeshRendererDeferred(vector<shared_ptr<GameObject>>& _gameObjects)
{
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;

	// 분류 단계 (투명 객체 제외)
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		// 투명 객체는 제외 (나중에 포워드로 처리)
		if (auto material = gameObject->GetMeshRenderer()->GetMaterial()) {
			if (material->IsTransparent())
				continue;
		}

		const InstanceID instanceID = gameObject->GetMeshRenderer()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	// 인스턴싱 렌더링
	for (auto& pair : cache) {
		const vector<shared_ptr<GameObject>>& vec = pair.second;
		const InstanceID instanceID = pair.first;

		for (int32 idx = 0; idx < vec.size(); ++idx) {
			const shared_ptr<GameObject>& gameObject = vec[idx];
			InstancingData data;
			data.m_world = gameObject->GetTransform()->GetWorldMatrix();
			AddData(instanceID, data);
		}

		shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

		// 디퍼드 렌더링 호출 (G-Buffer에 데이터 쓰기)
		vec[0]->GetMeshRenderer()->RenderInstancingDeferred(buffer, m_isShadowTech);
	}
}

void RenderManager::RenderModelRendererDeferred(vector<shared_ptr<GameObject>>& _gameObjects)
{
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;

	// 분류 단계 (투명 객체 제외)
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetModelRenderer() == nullptr)
			continue;

		// 투명 객체는 제외
		if (auto material = gameObject->GetModelRenderer()->GetMaterial()) {
			if (material->IsTransparent())
				continue;
		}

		const InstanceID instanceID = gameObject->GetModelRenderer()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	// 인스턴싱 렌더링
	for (auto& pair : cache) {
		const vector<shared_ptr<GameObject>>& vec = pair.second;
		const InstanceID instanceID = pair.first;

		for (int32 idx = 0; idx < vec.size(); ++idx) {
			const shared_ptr<GameObject>& gameObject = vec[idx];
			InstancingData data;
			data.m_world = gameObject->GetTransform()->GetWorldMatrix();
			AddData(instanceID, data);
		}

		shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

		// 디퍼드 렌더링 호출
		vec[0]->GetModelRenderer()->RenderInstancingDeferred(buffer, m_isShadowTech);
	}
}

void RenderManager::RenderAnimRendererDeferred(vector<shared_ptr<GameObject>>& _gameObjects)
{
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;

	// 분류 단계 (투명 객체 제외)
	for (shared_ptr<GameObject>& gameObject : _gameObjects) {
		if (gameObject->GetModelAnimator() == nullptr)
			continue;

		// 투명 객체는 제외
		auto modelAnimator = gameObject->GetModelAnimator();
		if (auto material = modelAnimator->GetMaterial()) {
			if (material->IsTransparent())
				continue;
		}

		const InstanceID instanceID = gameObject->GetModelAnimator()->GetInstanceID();
		cache[instanceID].push_back(gameObject);
	}

	// 인스턴싱 렌더링
	for (auto& pair : cache) {
		const vector<shared_ptr<GameObject>>& vec = pair.second;
		shared_ptr<InstancedTweenDesc> tweenDesc = make_shared<InstancedTweenDesc>();
		const InstanceID instanceID = pair.first;

		for (int32 idx = 0; idx < vec.size(); ++idx) {
			const shared_ptr<GameObject>& gameObject = vec[idx];
			InstancingData data;
			data.m_world = gameObject->GetTransform()->GetWorldMatrix();
			AddData(instanceID, data);

			// INSTANCING TWEEN 데이터 수집
			gameObject->GetModelAnimator()->UpdateTweenData();
			tweenDesc->tweens[idx] = gameObject->GetModelAnimator()->GetTweenDesc();
		}

		// G-Buffer 셰이더에 TweenDesc 전송
		if (vec[0]->GetModelAnimator()->GetGeometryShader()) {
			vec[0]->GetModelAnimator()->GetGeometryShader()->PushTweenData(*tweenDesc.get());
		}

		shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceID];

		// 디퍼드 렌더링 호출
		vec[0]->GetModelAnimator()->RenderInstancingDeferred(buffer, m_isShadowTech);
	}
}

void RenderManager::AddData(InstanceID _instanceID, InstancingData& _data)
{
	if (m_buffers.find(_instanceID) == m_buffers.end()) {
		m_buffers[_instanceID] = make_shared<InstancingBuffer>();
	}

	m_buffers[_instanceID]->AddData(_data);
}

void RenderManager::RenderGeometryPass(vector<shared_ptr<GameObject>>& _gameObjects)
{
	RenderMeshRendererDeferred(_gameObjects);
	RenderModelRendererDeferred(_gameObjects);
	RenderAnimRendererDeferred(_gameObjects);
}

void RenderManager::RenderDeferredLighting()
{
	if (m_deferredLightingShader == nullptr)
		return;

	GRAPHICS->BindGBufferSRVs();

	m_deferredLightingShader->GetSRV("ShadowMap")->SetResource(GRAPHICS->GetShadowMap()->GetComPtr().Get());

	auto lightObj = CURSCENE->GetLight();

	if (lightObj) {
		m_deferredLightingShader->PushLightData(lightObj->GetLight()->GetLightDesc());
	}

	m_deferredLightingShader->PushGlobalData(Camera::s_MatView, Camera::s_MatProjection);
	m_deferredLightingShader->PushFOWData(m_FogData);

	DC->OMSetDepthStencilState(nullptr, 0);
	DC->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	GRAPHICS->BindFullScreenQuad();
	DC->DrawIndexedInstanced(6, 1, 0, 0, 0);
	ID3D11ShaderResourceView* nullSRVs[Graphics::GBUFFER_COUNT] = {nullptr};
	DC->PSSetShaderResources(0, Graphics::GBUFFER_COUNT, nullSRVs);
	m_deferredLightingShader->GetSRV("ShadowMap")->SetResource(nullptr);
}

void RenderManager::RenderTransparentObjects(vector<shared_ptr<GameObject>>& _gameObjects)
{
	vector<shared_ptr<GameObject>> transparentObjects;

	// 투명 객체 선별 (기존 렌더러들을 통해 확인)
	for (auto& obj : _gameObjects) {
		bool isTransparent = false;

		// MeshRenderer 확인
		if (auto meshRenderer = obj->GetMeshRenderer()) {
			if (auto material = meshRenderer->GetMaterial()) {
				if (material->IsTransparent()) {
					isTransparent = true;
				}
			}
		}
		auto modelRenderer = obj->GetModelRenderer();
		// ModelRenderer 확인
		if (!isTransparent && modelRenderer) {
			if (auto material = modelRenderer->GetMaterial()) {
				if (material->IsTransparent()) {
					isTransparent = true;
				}
			}
		}
		auto modelAnimator = obj->GetModelAnimator();
		// ModelAnimator 확인
		if (!isTransparent && modelAnimator) {
			if (auto material = modelAnimator->GetMaterial()) {
				if (material->IsTransparent()) {
					isTransparent = true;
				}
			}
		}

		if (isTransparent) {
			transparentObjects.push_back(obj);
		}
	}

	if (transparentObjects.empty())
		return;

	// 깊이 정렬 (카메라로부터의 거리 기준)
	Vec3 cameraPos = Vec3(Camera::s_MatView._41, Camera::s_MatView._42, Camera::s_MatView._43);

	sort(transparentObjects.begin(), transparentObjects.end(),
		[cameraPos](const shared_ptr<GameObject>& a, const shared_ptr<GameObject>& b) {
		float distA = (a->GetTransform()->GetPosition() - cameraPos).Length();
		float distB = (b->GetTransform()->GetPosition() - cameraPos).Length();
		return distA > distB; // 먼 객체부터
	});

	// 포워드 렌더링으로 투명 객체들 렌더링
	RenderForward(transparentObjects, false);
}
