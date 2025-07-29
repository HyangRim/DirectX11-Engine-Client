#pragma once
#include "Engine/Scene.h"
#include "IExecute.h"
class GameObject;
class LumiaIsland :
    public Scene
{
	using Super = Scene;
public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;
	virtual void Render() override;


private:
	void CreateMainCamera();
	void CreateUICamera();

	//땅, 인테리어, 환경 생성. 
	void CreateCemeteryBase();
	void CreateCemeteryInterior();
	void CreateCemeteryEnvironment();
	void CreateCemeteryItemBox();
	// NavMesh 관련 추가
	void CreateNavMesh();
	void CreateCharacterNicky();
	void CreateCharacterBianca();


private:
	//테스트용. 
	void CreateTestDecal();

private:
	shared_ptr<GameObject> m_CemeteryParent;
	shared_ptr<Shader> m_defaultshader = nullptr;
	shared_ptr<Shader> m_testShader = nullptr;
	// 테스트용
	shared_ptr<GameObject> m_navMesh;
	shared_ptr<GameObject> nicky;
	shared_ptr<GameObject> bianca;
};


