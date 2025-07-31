#pragma once

#include "Engine/Scene.h"

class GameObject;
class CharacterSelectScene;

class StartScene :
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
	void CreateLight();

	void CreateLobbyBackGround();



	void LoadStartSceneImages();
	void LoadBtnImages();
	void LoadLobbyImages();

	void OnStartButtonClicked();

private:
	shared_ptr<Shader> m_defaultshader = nullptr;
	shared_ptr<Shader> m_imageShader = nullptr;

	shared_ptr<GameObject> m_backPanel = nullptr;
	shared_ptr<GameObject> m_button = nullptr;
	shared_ptr<GameObject> m_testPanel = nullptr;

	shared_ptr<GameObject> m_uiCamera = nullptr;


};


