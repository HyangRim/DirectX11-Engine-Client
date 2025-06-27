#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "Camera.h"
#include "Button.h"
#include "Sky.h"
#include "Light.h"
#include "Terrain.h"
void Scene::Start()
{
	//�浹 ���� �ʱ�ȭ. 
	m_mapColInfo.clear();

	const auto& objects = m_gameObjects;
	for (auto& object : m_gameObjects) {
		object->Start();
	}
}

void Scene::Update()
{
	const auto& objects = m_gameObjects;
	//Update �ڵ� �ȿ��� Remove�� ������� ������ ����� ������.
	//�̺�Ʈ ��ó���� ������Ʈ �߰�, ������ �ؾ���. 
	for (auto& object : m_gameObjects) {
		object->Update();
	}
	PickUI();
	Pick();

}

void Scene::FixedUpdate()
{
	const auto& objects = m_gameObjects;
	//Update �ڵ� �ȿ��� Remove�� ������� ������ ����� ������.
	//�̺�Ʈ ��ó���� ������Ʈ �߰�, ������ �ؾ���. 
	for (auto& object : m_gameObjects) {
		object->FixedUpdate();
	}
}



void Scene::LateUpdate()
{
	auto objects = m_gameObjects;
	for (auto& object : m_gameObjects) {
		object->LateUpdate();
	}
	CheckCollision();
}

void Scene::Render()
{
	for (auto camera : m_cameras) {
		//camera->GetCamera()->SortGameObject();
		//camera->GetCamera()->Render_Forward();
		//GRAPHICS->ClearDepthStencilView();

		Camera* cam = camera->GetCamera().get();
		if (cam->GetProjectionType() == ProjectionType::Perspective) {
			RenderGameCamera(cam);
		}
		else {
			RenderUICamera(cam);
		}
	}
}

void Scene::RenderGameCamera(Camera* cam)
{
	GRAPHICS->ClearShadowDepthStencilView();
	GRAPHICS->SetShadowDepthStencilView();

	Light* light = GetLight()->GetLight().get();

	cam->SetStaticData();
	cam->SortGameObject();

	if (light) {
		light->SetVPMatrix(cam, 100.0f, ::XMMatrixOrthographicLH(100, 100, 0, 200));
		cam->Render_Forward(true);
		Viewport& vp = GRAPHICS->GetShadowViewport();
		cam->Render_Backward(true);
	}

	GRAPHICS->SetRTVAndDSV();
	cam->Render_Forward(false);
	if (m_sky)
		m_sky->Render(cam);
	cam->Render_Backward(false);
}

void Scene::RenderUICamera(Camera* cam)
{
	GRAPHICS->ClearDepthStencilView();

	cam->SetStaticData();
	cam->SortGameObject();
	cam->Render_Forward(false);
	cam->Render_Backward(false);
}

void Scene::Add(shared_ptr<GameObject> _object)
{
	m_gameObjects.insert(_object);

	if (_object->GetCamera() != nullptr) {
		m_cameras.insert(_object);
	}

	if (_object->GetLight() != nullptr) {
		m_Lights.insert(_object);
	}
}

void Scene::Remove(shared_ptr<GameObject> _object)
{
	m_gameObjects.erase(_object);
	m_cameras.erase(_object);
	m_Lights.erase(_object);
}

shared_ptr<GameObject> Scene::GetMainCamera()
{
	for (auto camera : m_cameras) {
		if (camera->GetCamera()->GetProjectionType() == ProjectionType::Perspective)
			return camera;
	}
	return nullptr;
}

shared_ptr<GameObject> Scene::GetUICamera()
{
	for (auto camera : m_cameras) {
		if (camera->GetCamera()->GetProjectionType() == ProjectionType::Orthographic)
			return camera;
	}
	return nullptr;
}

void Scene::PickUI()
{
	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON) == false) {
		return;
	}

	if (GetUICamera() == nullptr)
		return;

	POINT screenpPt = INPUT->GetMousePos();

	shared_ptr<Camera> camera = GetUICamera()->GetCamera();
	const auto gameObjects = GetObjects();

	for (auto& object : gameObjects) {
		if (object->GetButton() == nullptr)
			continue;

		if (object->GetButton()->Picked(screenpPt))
			object->GetButton()->InvokeOnClicked();
	}
}

shared_ptr<class GameObject> Scene::Pick()
{
	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON) == false)
		return nullptr;
	//int32 _screenX, int32 _screenY
	int screenX = INPUT->GetMousePos().x;
	int screenY = INPUT->GetMousePos().y;

	shared_ptr<Camera> camera = GetMainCamera()->GetCamera();

	float width = GRAPHICS->GetViewport().GetWidth();
	float height = GRAPHICS->GetViewport().GetHeight();


	Matrix projectionMatrix = camera->GetProjectionMatrix();

	//View��ǥ�� ��ȯ
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	//View�� ��ȯ�ϴ� ����� ���ϱ�. 
	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	//��� ������Ʈ ���ϱ�. 
	const auto& gameObjects = GetObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	// ViewSpace���� Ray ����
	// ViewSpace�� ī�޶��� ��ġ�� �����̱� ������ origin�� 0����.
	// rayDir�� �������� ������ �����شٴ� ��. 
	Vec4 rayOrigin = Vec4(0.f, 0.f, 0.f, 1.f);
	Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.f);

	//View���� ���� -> world�� ���ư���. 
	//��ġ�� Dir���ϱ�. 
	Vec3 worldRayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
	Vec3 worldRayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
	worldRayDir.Normalize();

	Ray ray = Ray(worldRayOrigin, worldRayDir);




	//��� ��ü�� ���� �� ��ĵ�ϴ� ������ ���. 
	for (auto& gameObject : gameObjects) {
		if(camera->IsCulled(gameObject->GetLayerIndex()))
			continue;
		//Collider �ٿ��߸� �ǰ� �ȴ�. 
		if (gameObject->GetCollider() == nullptr)
			continue;


		//WorldSpace���� �����ϱ�. 
		Ray ray = Ray(worldRayOrigin, worldRayDir);

		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(ray, OUT distance) == false)
			continue;

		if (distance < minDistance) {
			minDistance = distance;
			picked = gameObject;
		}
	}


	//Teraain Ŭ�� �˻�. 
	for (auto& gameObject : gameObjects) {
		if (gameObject->GetTerrain() == nullptr)
			continue;

		Vec3 pickPos;
		float distance = 0.0f;
		if (gameObject->GetTerrain()->Pick(screenX, screenY, OUT pickPos, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	if (picked) {
		std::cout << "picked\n";
	}

	return picked;

}

void Scene::CheckCollision()
{

	//1. m_gameObjects���� for�� ������ �˻��Ѵ�. 
	//2. Collider���� ��, obj�� �޴´�. 
	vector<shared_ptr<BaseCollider>> colliders;
	for (auto object : m_gameObjects) {
		if (object->GetCollider() == nullptr)
			continue;

		colliders.push_back(object->GetCollider());
	}

	//BruteForce
	//���� Ʈ�� ���� ��. 

	//Collider���� �˻�. -> GameObject�� Collisionó�� �ؾ��ϴµ�. 
	for (uint32 idx = 0; idx < colliders.size(); ++idx) {
		if (colliders[idx].get()->GetActive() == false)
			continue;

		for (uint32 jdx = idx + 1; jdx < colliders.size(); ++jdx) {
			shared_ptr<BaseCollider>& other = colliders[jdx];

			if (other.get()->GetActive() == false)
				continue;


			COLLIDER_ID id;
			id.left_id = colliders[idx].get()->GetID();
			id.right_id = colliders[jdx].get()->GetID();

			auto colliderMapIter = m_mapColInfo.find(id.ID);

			//�浹 ������ �� ��� ������ ���.(�浹���� �ʾҴٷ� �Է�.) 
			if (colliderMapIter == m_mapColInfo.end()) {
				m_mapColInfo.insert(make_pair(id.ID, false));
				colliderMapIter = m_mapColInfo.find(id.ID);
			}

			//���� Collider���� �浹���� ��쿡. 
			if (colliders[idx]->Intersects(other)) {
				
				if (colliderMapIter->second == false) {
					//�̹� �����ӿ� �� �浹�� ���.

					//TODO : �̺�Ʈ ��ó�� �ý��� ���? 
					colliders[idx].get()->GetGameObject()->OnCollisionEnter(colliders[jdx].get()->GetGameObject());
					colliders[jdx].get()->GetGameObject()->OnCollisionEnter(colliders[idx].get()->GetGameObject());
					colliderMapIter->second = true;
				}
				else {
					//���� �����ӿ��� �浹�ϰ� �ִ� ���. 
					colliders[idx].get()->GetGameObject()->OnCollision(colliders[jdx].get()->GetGameObject());
					colliders[jdx].get()->GetGameObject()->OnCollision(colliders[idx].get()->GetGameObject());
				}
			}
			else {
				//�浹���� �ʾ��� ���. 
				if (colliderMapIter->second == true) {
					//������ �浹�ϰ� �־�����. 
					colliders[idx].get()->GetGameObject()->OnCollisionExit(colliders[jdx].get()->GetGameObject());
					colliders[jdx].get()->GetGameObject()->OnCollisionExit(colliders[idx].get()->GetGameObject());
					colliderMapIter->second = false;
				}
			}
		}
	}
}
