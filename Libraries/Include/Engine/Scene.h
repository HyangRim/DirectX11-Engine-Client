#pragma once

class Sky;
class Camera;

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

	void SetSky(shared_ptr<Sky> _sky) { m_sky = _sky; }

	void CheckCollision();

private:
	//�浹 ���� HashTable
	//�浹ü ���� ���� ������ �浹. 
	unordered_map<ULONG64, bool> m_mapColInfo;

	//�׷찣�� �浹 üũ? �ϴ� ����. 

private:
	unordered_set<shared_ptr<GameObject>> m_gameObjects;
	//Cache Camera;
	unordered_set<shared_ptr<GameObject>> m_cameras;
	//Cache Light;
	unordered_set<shared_ptr<GameObject>> m_Lights;

	shared_ptr<Sky> m_sky;
};

