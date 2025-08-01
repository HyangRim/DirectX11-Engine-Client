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


	//=====================UI관련 함수=====================//
	void LoadItemBoxImages();
	void CreateItemBoxPanel();
	void CheckPickedItemBox();

	void LoadCharStatIcon();
	void CreateCharStatPanel();

	void LoadCharEquipmentIcon();
	void CreateCharEquipmentPanel();

	void LoadCharMainImages();
	void CreateCharMainPanel();

	void LoadCharInventoryImages();
	void CreateCharInventoryPanel();

	//=====================UI관련 함수=====================//



	Vec4 ColorNormalize(Vec4 input);	//RGBA ( 0 ~ 255 ) 넣으면 -> ( 0 ~ 1 )

private:
	//테스트용. 
	void CreateTestDecal();

	void CreateTestMesh();

private:
	shared_ptr<GameObject> m_CemeteryParent;
	shared_ptr<Shader> m_defaultshader = nullptr;
	shared_ptr<Shader> m_testShader = nullptr;

	//=====================UI관련 변수=====================//
	shared_ptr<GameObject> m_itemBox = nullptr;

	shared_ptr<GameObject> m_charStatPanel = nullptr;
	shared_ptr<GameObject> m_charEquipPanel = nullptr;
	shared_ptr<GameObject> m_charMainPanel = nullptr;
	shared_ptr<GameObject> m_charInventoryPanel = nullptr;
	//=====================UI관련 변수=====================//

	// 테스트용
	shared_ptr<GameObject> m_navMesh;
	shared_ptr<GameObject> nicky;
	shared_ptr<GameObject> bianca;
};


