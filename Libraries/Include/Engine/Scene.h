#pragma once

class Scene
{
public:
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();

	virtual void Render();


	virtual void Add(shared_ptr<GameObject> _object);
	virtual void Remove(shared_ptr<GameObject> _object);


	const unordered_set<shared_ptr<GameObject>>& GetObjects() { return m_gameObjects; }
	shared_ptr<GameObject> GetMainCamera();
	shared_ptr<GameObject> GetUICamera();
	shared_ptr<GameObject> GetLight() { return m_Lights.empty() ? nullptr : *m_Lights.begin(); }

	void PickUI();
	shared_ptr<class GameObject> Pick(int32 _screenX, int32 _screenY);

	void CheckCollision();

private:
	unordered_set<shared_ptr<GameObject>> m_gameObjects;
	//일단 임시로 만들어놓음. 
	//Cache Camera;
	unordered_set<shared_ptr<GameObject>> m_cameras;

	//Cache Light;
	unordered_set<shared_ptr<GameObject>> m_Lights;
};

