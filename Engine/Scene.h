#pragma once

class Scene
{
public:
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();


	virtual void Add(shared_ptr<GameObject> _object);
	virtual void Remove(shared_ptr<GameObject> _object);
private:
	unordered_set<shared_ptr<GameObject>> m_gameObjects;


	//일단 임시로 만들어놓음. 
	//Cache Camera;
	unordered_set<shared_ptr<GameObject>> m_cameras;

	//Cache Light;
	unordered_set<shared_ptr<GameObject>> m_Lights;
};

