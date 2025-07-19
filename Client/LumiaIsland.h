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

private:
	shared_ptr<GameObject> m_CemeteryParent;
	shared_ptr<Shader> m_defaultshader = nullptr;
};


