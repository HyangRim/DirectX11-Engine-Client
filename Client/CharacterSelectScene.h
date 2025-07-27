#pragma once

#include "Engine/Scene.h"

class GameObject;

class CharacterSelectScene :
    public Scene
{
public:
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();
	virtual void LateUpdate();
	virtual void Render();


private:
	void CreateMainCamera();
	void CreateUICamera();

	void CreateBackGround(); 
	void CreateScrollableCharacterList();

	void LoadCharacterSelectSceneImages();
	void LoadBackGround();
	void LoadCharacterSlotImages();


private:
	shared_ptr<Shader> m_defaultshader = nullptr;
	shared_ptr<Shader> m_imageShader = nullptr;

	shared_ptr<GameObject> m_backPanel = nullptr;
};


