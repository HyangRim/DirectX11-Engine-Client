#pragma once

class Sky;
class Camera;
class BaseCollider;
#include "QuadTree.h"

union COLLIDER_ID {
	struct {
		uint32 left_id;
		uint32 right_id;
	};
	ULONG64 ID;
};

class Scene
{

public:
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();
	virtual void LateUpdate();

	virtual void Render();

	void RenderGameCamera(Camera* cam);
	void RenderUICamera(Camera* cam);


	virtual void Add(shared_ptr<GameObject> _object);
	virtual void Remove(shared_ptr<GameObject> _object);


	const unordered_set<shared_ptr<GameObject>>& GetObjects() { return m_gameObjects; }
	shared_ptr<GameObject> GetMainCamera();
	shared_ptr<GameObject> GetUICamera();
	shared_ptr<GameObject> GetLight() { return m_Lights.empty() ? nullptr : *m_Lights.begin(); }
	


	void PickUI();
	shared_ptr<class GameObject> Pick();
	shared_ptr<class GameObject> PickObjectOrUI();
	string ws2s(const wstring& wstr);

	void SetSky(shared_ptr<Sky> _sky) { m_sky = _sky; }

	void CheckCollision();
	void CheckCollisionWithQuadTree();

///////쿼드 트리 테스트용//////////////

private:
	unique_ptr<QuadTree> m_quadTree;
	bool m_quadTreeDirty = true;

public:
	void UpdateQuadTree();
	void GameObjectsTest();
	QuadTree* GetQuadTree() { return m_quadTree.get(); }
	Ray CreateRayFromScreen(const Vec2& screenPos, shared_ptr<Camera> camera);

	weak_ptr<GameObject> GetPickedObj() { return m_curPickedObj; }
	
///////////////////////////////////////

private:
	//충돌 관련 HashTable
	//충돌체 간의 이전 프레임 충돌. 
	unordered_map<ULONG64, bool> m_mapColInfo;

	//그룹간의 충돌 체크? 일단 보류. 

private:
	unordered_set<shared_ptr<GameObject>> m_gameObjects;
	//Cache Camera;
	unordered_set<shared_ptr<GameObject>> m_cameras;
	//Cache Light;
	unordered_set<shared_ptr<GameObject>> m_Lights;

	shared_ptr<Sky> m_sky;
	weak_ptr<GameObject> m_curPickedObj;
};

