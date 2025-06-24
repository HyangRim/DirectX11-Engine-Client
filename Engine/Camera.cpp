#include "pch.h"
#include "Camera.h"
#include "Component.h"
#include "Transform.h"
#include "Scene.h"
#include "Renderer.h"
#include "Material.h"


Matrix Camera::s_MatView = Matrix::Identity;
Matrix Camera::s_MatProjection = Matrix::Identity;

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



//ī�޶�� �����ִ� �� View�� Projection.

//View�� �翬�� ī�޶� ��ǥ ���ؿ��� �����ϴϱ�.
//Projection�� 
void Camera::UpdateMatrix()
{
	Vec3 eyePosition = GetTransform()->GetPosition();
	Vec3 focusPosition = eyePosition + GetTransform()->GetLook();
	Vec3 upDirection = GetTransform()->GetUp();
	
	m_matView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);


	if (m_type == ProjectionType::Perspective) {
		m_matProjection = s_MatProjection = ::XMMatrixPerspectiveFovLH(m_fov, m_width / m_height, m_near, m_far);
	}
	else {
		m_matProjection = s_MatProjection = ::XMMatrixOrthographicLH(m_width, m_height, m_near, m_far);
	}
}

void Camera::SortGameObject()
{
	shared_ptr<Scene> scene = CURSCENE;
	const unordered_set<shared_ptr<GameObject>>& gameObjects = scene->GetObjects();

	m_vecForward.clear();
	m_vecBackward.clear();

	//�׷��� �� �����ϱ�. 
	for (auto& object : gameObjects) {
		if (IsCulled(object->GetLayerIndex()))
			continue;

		shared_ptr<Renderer> renderer = object->GetRenderer();
		if (renderer == nullptr)
			continue;

		shared_ptr<Material> material = renderer->GetMaterial();
		RenderQueue renderQueue = material->GetRenderQueue();

		//TODO : �ƾƿ��� �����ϱ�
		//TODO : �Ÿ��� ���� �����ϱ�

		switch (renderQueue) 
		{
		case RenderQueue::Opaque:
			m_vecForward.push_back(object);
			break;
		case RenderQueue::Transparent:
			m_vecBackward.push_back(object);
			break;
		}
	}
}

void Camera::Render_Forward()
{
	s_MatView = m_matView;
	s_MatProjection = m_matProjection;

	RENDER->Render(m_vecForward);
}

void Camera::Render_Backward()
{
	RENDER->Render(m_vecBackward);
}
