#include "pch.h"
#include "Camera.h"
#include "Component.h"
#include "Transform.h"
#include "Scene.h"
#include "Renderer.h"
#include "Material.h"
#include "QuadTree.h"
#include "GameObject.h"
#include "IFogOfWar.h"
#include "MonoBehaviour.h"


Matrix Camera::s_MatView = Matrix::Identity;
Matrix Camera::s_MatProjection = Matrix::Identity;
Vec3 Camera::s_Pos = Vec3::Zero;

Camera::Camera() : Super(ComponentType::Camera)
{
	m_width = static_cast<float>(GAME->GetGameDesc().width);
	m_height = static_cast<float>(GAME->GetGameDesc().height);
}

Camera::~Camera()
{

}

void Camera::LateUpdate()
{
	UpdateMatrix();

	//RENDER->PushGlobalData(Camera::s_MatView, Camera::s_MatProjection);
}


//카메라랑 연관있는 건 View와 Projection.

//View는 당연히 카메라 좌표 기준에서 봐야하니까.
//Projection은 
void Camera::UpdateMatrix()
{
	Vec3 eyePosition = GetTransform()->GetPosition();
	Vec3 focusPosition = eyePosition + GetTransform()->GetLook();
	Vec3 upDirection = GetTransform()->GetUp();
	
	m_matView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);

	if (m_type == ProjectionType::Perspective) 
	{
		m_matProjection = s_MatProjection = ::XMMatrixPerspectiveFovLH(m_fov, m_width / m_height, m_near, m_far);
	}
	else 
	{
		m_matProjection = s_MatProjection = ::XMMatrixOrthographicLH(m_width, m_height, m_near, m_far);
	}
}

void Camera::SortGameObject()
{
	// ProjectionType에 따라 분기
	if (m_type == ProjectionType::Perspective) {
		SortGameObjects();  // 일반 게임 객체 렌더링
	}
	else if (m_type == ProjectionType::Orthographic) {
		SortUIObjects();    // UI 객체 렌더링
	}
}

void Camera::SortGameObjects()
{
	shared_ptr<Scene> scene = CURSCENE;
	const unordered_set<shared_ptr<GameObject>>& gameObjects = scene->GetObjects();

	m_vecForward.clear();
	m_vecBackward.clear();

	// FOW 인터페이스 캐싱 시스템 (성능 최적화)
	static IFogOfWar* cachedFogOfWar = nullptr;
	static int lastFrameCheck = -1;
	int currentFrame = GetTickCount64() / 16;

	if (lastFrameCheck != currentFrame) {
		cachedFogOfWar = nullptr;

		for (auto& obj : gameObjects) {
			auto scripts = obj->GetScripts();
			for (auto& comp : scripts) {
				IFogOfWar* fogInterface = dynamic_cast<IFogOfWar*>(comp.get());
				if (fogInterface) {
					cachedFogOfWar = fogInterface;
					break;
				}
			}
			if (cachedFogOfWar) break;
		}
		lastFrameCheck = currentFrame;
	}

	if (cachedFogOfWar) {
		cachedFogOfWar->UpdateFOWSystem();
	}

	FOW->UpdateShaderConstants();

	// **일반 게임 객체만 처리**
	for (auto& object : gameObjects) {
		if (IsCulled(object->GetLayerIndex()))
			continue;

		// QuadTree를 통한 Frustum Culling
		if (scene->GetQuadTree()->IsObjectVisible(object, this) == false) {
			continue;
		}

		// FOW 체크
		if (cachedFogOfWar) {
			if (!cachedFogOfWar->ShouldRenderObject(object)) {
				continue;
			}
			float alpha = cachedFogOfWar->GetObjectAlpha(object);
			object->SetAlpha(alpha);
		}

		shared_ptr<Renderer> renderer = object->GetRenderer();
		if (renderer == nullptr)
			continue;

		shared_ptr<Material> material = renderer->GetMaterial();
		RenderQueue renderQueue = material->GetRenderQueue();

		switch (renderQueue) {
		case RenderQueue::Opaque:
		case RenderQueue::Cutout:
			m_vecForward.push_back(object);
			break;
		case RenderQueue::Transparent:
			m_vecBackward.push_back(object);
			break;
		}
	}
}

void Camera::SortUIObjects()
{
	shared_ptr<Scene> scene = CURSCENE;

	// **UI 객체만 가져오기**
	const unordered_set<shared_ptr<GameObject>>& uiObjects = scene->m_uiObjects;

	m_vecForward.clear();
	m_vecBackward.clear();

	// UI 객체들을 Z 순서로 정렬하여 렌더링
	vector<shared_ptr<GameObject>> sortedUIObjects(uiObjects.begin(), uiObjects.end());

	// Z 좌표 기준으로 정렬 (멀리 있는 것부터 가까운 순으로)
	std::sort(sortedUIObjects.begin(), sortedUIObjects.end(),
		[](const shared_ptr<GameObject>& a, const shared_ptr<GameObject>& b) {
			return a->GetTransform()->GetPosition().z > b->GetTransform()->GetPosition().z;
		});

	for (auto& object : sortedUIObjects) {
		if (IsCulled(object->GetLayerIndex()))
			continue;

		shared_ptr<Renderer> renderer = object->GetRenderer();
		if (renderer == nullptr)
			continue;

		shared_ptr<Material> material = renderer->GetMaterial();
		RenderQueue renderQueue = material->GetRenderQueue();

		switch (renderQueue) {
		case RenderQueue::Opaque:
		case RenderQueue::Cutout:
			m_vecForward.push_back(object);
			break;
		case RenderQueue::Transparent:
			m_vecBackward.push_back(object);
			break;
		}
	}
}

void Camera::SetStaticData() {
	s_MatView = m_matView;
	s_MatProjection = m_matProjection;
	s_Pos = GetTransform()->GetPosition();
}

void Camera::Render_Forward(bool _isShadowTech)
{
	RENDER->Render(m_vecForward, _isShadowTech);
}

void Camera::Render_Backward(bool _isShadowTech)
{
	RENDER->Render(m_vecBackward, _isShadowTech);
}
