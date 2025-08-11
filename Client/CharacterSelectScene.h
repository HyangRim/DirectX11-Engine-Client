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
	void CreateLight();

	void CreateBackGround(); 
	void CreateScrollableCharacterList();
	void CreateScrollableSkinList(); // 스킨 목록 + full image
	
	void UpdateSkinList(shared_ptr<Button> button, int charIndex);
	void UpdateFullImage(shared_ptr<Button> button, int skinIndex);

	void LoadCharacterSelectSceneImages();

	void LoadBackGround();
	void LoadCharacterListSlotImages();
	void LoadCharacterSkinListSlotImages();
	void LoadCharacterImages();
	void LoadCharacterFullAndHalfImages();


	void OnCharacterSelectButtonClicked(int charIndex);
	void OnCharacterSelectButtonHover();

private:
	shared_ptr<Shader> m_defaultshader = nullptr;
	shared_ptr<Shader> m_imageShader = nullptr;
	
	shared_ptr<GameObject> m_backPanel = nullptr;
	shared_ptr<GameObject> m_characterList = nullptr;
	shared_ptr<GameObject> m_selectedCharacterSkinScrollView = nullptr;
};


