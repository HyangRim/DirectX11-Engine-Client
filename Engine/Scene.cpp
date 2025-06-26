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
	auto objects = m_gameObjects;
	for (auto object : m_gameObjects) {
		object->Start();
	}
}

void Scene::Update()
{
	auto objects = m_gameObjects;
	//Update 코드 안에서 Remove를 써버리면 문제가 생기기 때문에.
	//이벤트 후처리로 오브젝트 추가, 생성을 해야함. 
	for (auto object : m_gameObjects) {
		object->Update();
	}
	PickUI();
	Pick();

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

	//View좌표로 변환
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	//View로 변환하는 역행렬 구하기. 
	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	//모든 오브젝트 구하기. 
	const auto& gameObjects = GetObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	// ViewSpace에서 Ray 정의
	// ViewSpace란 카메라의 위치가 원점이기 때문에 origin이 0벡터.
	// rayDir은 시작점과 끝점을 구해준다는 것. 
	Vec4 rayOrigin = Vec4(0.f, 0.f, 0.f, 1.f);
	Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.f);

	//View시점 원점 -> world로 돌아가기. 
	//위치와 Dir구하기. 
	Vec3 worldRayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
	Vec3 worldRayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
	worldRayDir.Normalize();

	Ray ray = Ray(worldRayOrigin, worldRayDir);




	//모든 물체를 전부 다 스캔하는 무식한 방법. 
	for (auto& gameObject : gameObjects) {
		if(camera->IsCulled(gameObject->GetLayerIndex()))
			continue;
		//Collider 붙여야만 피격 된다. 
		if (gameObject->GetCollider() == nullptr)
			continue;


		//WorldSpace에서 연산하기. 
		Ray ray = Ray(worldRayOrigin, worldRayDir);

		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(ray, OUT distance) == false)
			continue;

		if (distance < minDistance) {
			minDistance = distance;
			picked = gameObject;
		}
	}


	//Teraain 클릭 검사. 
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
	vector<shared_ptr<BaseCollider>> colliders;
	for (auto object : m_gameObjects) {
		if (object->GetCollider() == nullptr)
			continue;

		colliders.push_back(object->GetCollider());
	}

	//BruteForce
	//쿼드 트리 같은 거. 
	for (uint32 idx = 0; idx < colliders.size(); ++idx) {
		for (uint32 jdx = idx + 1; jdx < colliders.size(); ++jdx) {
			shared_ptr<BaseCollider>& other = colliders[jdx];


			if (colliders[idx]->Intersects(other)) {
				//충돌 관련 처리. 
			}
		}
	}
}
