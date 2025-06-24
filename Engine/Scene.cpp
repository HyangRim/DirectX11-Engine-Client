#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "Camera.h"
#include "Button.h"
#include "Sky.h"
void Scene::Start()
{
	auto objects = m_gameObjects;
	for (auto object : m_gameObjects) {
		object->Start();
	}
}

void Scene::Update()
{
	auto objects = m_gameObjects;
	//Update �ڵ� �ȿ��� Remove�� ������� ������ ����� ������.
	//�̺�Ʈ ��ó���� ������Ʈ �߰�, ������ �ؾ���. 
	for (auto object : m_gameObjects) {
		object->Update();
	}
	PickUI();

}

void Scene::LateUpdate()
{
	auto objects = m_gameObjects;
	for (auto object : m_gameObjects) {
		object->LateUpdate();
	}
	CheckCollision();
}

void Scene::Render()
{
	for (auto camera : m_cameras) {
		//camera->GetCamera()->SortGameObject();
		//camera->GetCamera()->Render_Forward();

		const shared_ptr<Camera>& cam = camera->GetCamera();
		cam->SortGameObject();
		cam->Render_Forward();
		if (cam->IsCulled(LAYER_UI) == true && m_sky)
			m_sky->Render(cam);

		cam->Render_Backward();
	}
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

shared_ptr<class GameObject> Scene::Pick(int32 _screenX, int32 _screenY)
{
	shared_ptr<Camera> camera = GetMainCamera()->GetCamera();

	float width = GRAPHICS->GetViewport().GetWidth();
	float height = GRAPHICS->GetViewport().GetHeight();


	Matrix projectionMatrix = camera->GetProjectionMatrix();

	//View��ǥ�� ��ȯ
	float viewX = (+2.0f * _screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * _screenY / height + 1.0f) / projectionMatrix(1, 1);

	//View�� ��ȯ�ϴ� ����� ���ϱ�. 
	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	//��� ������Ʈ ���ϱ�. 
	const auto& gameObjects = GetObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;


	//��� ��ü�� ���� �� ��ĵ�ϴ� ������ ���. 
	for (auto& gameObject : gameObjects) {
		if(camera->IsCulled(gameObject->GetLayerIndex()))
			continue;
		//Collider �ٿ��߸� �ǰ� �ȴ�. 
		if (gameObject->GetCollider() == nullptr)
			continue;

		// ViewSpace���� Ray ����
		// ViewSpace�� ī�޶��� ��ġ�� �����̱� ������ origin�� 0����.
		// rayDir�� �������� ������ �����شٴ� ��. 
		Vec4 rayOrigin = Vec4(0.f, 0.f, 0.f, 1.f);
		Vec4 rayDir = Vec4(viewX, viewX, 1.0f, 0.f);

		//View���� ���� -> world�� ���ư���. 
		//��ġ�� Dir���ϱ�. 
		Vec3 worldRayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		Vec3 worldRayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		worldRayDir.Normalize();

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

	return picked;

}

void Scene::CheckCollision()
{
	vector<shared_ptr<BaseCollider>> colliders;

	for (auto object : m_gameObjects) {
		if (object->GetCollider() == nullptr)
			continue;

		colliders.push_back(object->GetCollider());
	}

	//BruteForce
	//���� Ʈ�� ���� ��. 
	for (uint32 idx = 0; idx < colliders.size(); ++idx) {
		for (uint32 jdx = 0; jdx < colliders.size(); ++jdx) {
			shared_ptr<BaseCollider>& other = colliders[jdx];

			if (colliders[idx]->Intersects(other)) {
				int a = 3;
			}
		}
	}
}
