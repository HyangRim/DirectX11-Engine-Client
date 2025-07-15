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
    shared_ptr<Scene> scene = CURSCENE;
    const unordered_set<shared_ptr<GameObject>>& gameObjects = scene->GetObjects();
    const unordered_set<shared_ptr<GameObject>>& uiObjects = scene->m_uiObjects;

    m_vecForward.clear();
    m_vecBackward.clear();

    // FOW 인터페이스 캐싱 시스템 (성능 최적화)
    static IFogOfWar* cachedFogOfWar = nullptr;
    static int lastFrameCheck = -1;
    int currentFrame = GetTickCount64() / 16; // 60FPS 기준

    if (lastFrameCheck != currentFrame) {
        cachedFogOfWar = nullptr;

        // IFogOfWar 인터페이스 구현체 찾기
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

    // FOW 시스템 업데이트 (엔진에서 호출)
    if (cachedFogOfWar) {
        cachedFogOfWar->UpdateFOWSystem();
    }

    if (m_type == ProjectionType::Perspective)
    {
        int CullingObject = 0;


        //cout << "전체 오브젝트 : " << gameObjects.size() << "\n";
        //그려줄 것 선별하기. 
        for (auto& object : gameObjects)
        {
            if (object->GetType() != OBJECTTYPE::MAP) {
                //레이어 컬링. 
                if (IsCulled(object->GetLayerIndex()))
                    continue;

                // QuadTree를 통한 Frustum Culling.
                if (!scene->GetQuadTree()->IsObjectVisible(object, this))
                {
                    CullingObject++;
                    continue;
                }

                //FOW통한 컬링. 
                if (cachedFogOfWar) {
                    if (!cachedFogOfWar->ShouldRenderObject(object)) {
                        continue;
                    }

                    float alpha = cachedFogOfWar->GetObjectAlpha(object);
                    object->SetAlpha(alpha);
                }
            }


            //QuadTree - Visible가지고 Frustum Culling 가능. 
            shared_ptr<Renderer> renderer = object->GetRenderer();
            if (renderer == nullptr)
                continue;

            shared_ptr<Material> material = renderer->GetMaterial();
            RenderQueue renderQueue = material->GetRenderQueue();

            //TODO : 컷아웃용 정렬하기
            //TODO : 거리에 따라 정렬하기

            switch (renderQueue)
            {
            case RenderQueue::Opaque:
            case RenderQueue::Cutout:
                m_vecForward.push_back(object);
                break;
            case RenderQueue::Transparent:
                m_vecBackward.push_back(object);
                break;

            }

            //cout << "컬링 오브젝트 : " << CullingObject << "\n";
            //cout << "렌더링 오브젝트 : " << gameObjects.size() - CullingObject << "\n";
        }
    }
    else
    {
        //그려줄 것 선별하기. 
        for (auto& object : uiObjects)
        {
            if (IsCulled(object->GetLayerIndex()))
                continue;

            //QuadTree - Visible가지고 Frustum Culling 가능. 
            shared_ptr<Renderer> renderer = object->GetRenderer();
            if (renderer == nullptr)
                continue;

            shared_ptr<Material> material = renderer->GetMaterial();

            RenderQueue renderQueue = material->GetRenderQueue();

            //TODO : 컷아웃용 정렬하기
            //TODO : 거리에 따라 정렬하기
            switch (renderQueue)
            {
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
