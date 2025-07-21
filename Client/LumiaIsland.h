#pragma once
#include "IExecute.h"
class GameObject;
class LumiaIsland :
    public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;


private:
	void CreateMainCamera();
	void CreateUICamera();

	//땅, 인테리어, 환경 생성. 
	void CreateCemeteryBase();
	void CreateCemeteryInterior();
	void CreateCemeteryEnvironment();
	// NavMesh 관련 추가
	void CreateNavMesh();
	void CreateCharacter();

private:
	shared_ptr<GameObject> m_CemeteryParent;
	shared_ptr<Shader> m_defaultshader = nullptr;
	shared_ptr<Shader> m_testShader = nullptr;
	// 테스트용
	shared_ptr<GameObject> m_navMesh;
	shared_ptr<GameObject> nicky;
};


