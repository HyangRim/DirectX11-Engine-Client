#include "pch.h"
#include "FogOfWar.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"
#include "QuadTree.h"
#include "Renderer.h"
#include "Material.h"

FogOfWar::FogOfWar()
{
}

FogOfWar::~FogOfWar()
{
}

void FogOfWar::Init()
{
	Super::Init();

	FOW->Init();
	ApplyToMapObjects();
}

void FogOfWar::Update()
{

	Super::Update();

	m_curTime += DT;

	if (m_curTime > m_updateTime) {
		m_curTime = 0.f;
		m_needsUpdate = false;
	}
}

bool FogOfWar::ShouldRenderObject(shared_ptr<GameObject> _object)
{
	if (!_object) return false;

	if (IsMapObject(_object))return true;
	if (_object == GetGameObject()) return true;

	Vec3 playerPos = GetTransform()->GetPosition();
	Vec3 objPos = _object->GetTransform()->GetPosition();

	float distance = Vec3::Distance(playerPos, objPos);

	return distance <= m_sightRange;
}

float FogOfWar::GetObjectAlpha(shared_ptr<GameObject> _object)
{
	Vec3 playerPos = GetTransform()->GetPosition();
	Vec3 objPos = _object->GetTransform()->GetPosition();
	float distance = Vec3::Distance(playerPos, objPos);

	if (distance <= m_sightRange - m_fadeDistance) {
		return 1.0f;
	}
	else if (distance <= m_sightRange) {
		float fadeRatio = (distance - (m_sightRange - m_fadeDistance)) / m_fadeDistance;
		return 1.0f - fadeRatio;
	}
	else {
		return 0.0f;
	}
}

void FogOfWar::UpdateFOWSystem()
{
	UpdateFOWShader();
}



void FogOfWar::UpdateFOWShader()
{
	FogOfWarData fowData = {};
	fowData.playerWorldPos = GetTransform()->GetPosition();
	fowData.sightRange = m_sightRange;
	fowData.darkness = m_darkness;
	fowData.fadeDistance = m_fadeDistance;
	fowData.smoothness = m_smoothness;

	FOW->SetFogOfWarData(fowData);
}

void FogOfWar::ApplyToMapObjects()
{
	const auto& objects = CURSCENE->GetObjects();

	for (auto& obj : objects) {
		if (IsMapObject(obj)) {
			auto renderer = obj->GetRenderer();
			if (renderer) {
				FOW->ApplyToMaterial(renderer->GetMaterial());
			}
		}
	}
}

bool FogOfWar::IsMapObject(shared_ptr<GameObject> _object)
{
	wstring name = _object->GetName();
	return (name.find(L"Map") != wstring::npos ||
		name.find(L"Terrain") != wstring::npos ||
		_object->GetLayerIndex() == 0);
}


