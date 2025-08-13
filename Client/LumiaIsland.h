#pragma once
#include "Engine/Scene.h"
#include "IExecute.h"

#include "InventoryManager.h"
#include "ItemSlot.h"

class GameObject;
class Player;
class Cursor;
class BiancaCamera;

class LumiaIsland :
    public Scene
{
	using Super = Scene;

public:
	LumiaIsland();
	virtual ~LumiaIsland();
public:

	virtual void Start() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

public:
	void SetSelectedCharacter(int _idx) { m_selectedCharacterIdx = _idx; }

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

	void CreateMonsterWolf(Vec3 _pos);
	void CreateMonsterAlpha(Vec3 _pos);

	//=====================UI관련 함수=====================//
	void CreateCursor();

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

	void LoadTimeImage();
	void CreateTimePanel();
	void CreateDayPanel();

	void UpdateSkillCoolDown();
	void UpdatePlayerStatus();
	void UpdateHPAndSPBar();
	void UpdateTimeline();

	void UpdatePlayerLevel();
	//=====================UI관련 함수=====================//



	Vec4 ColorNormalize(Vec4 input);	//RGBA ( 0 ~ 255 ) 넣으면 -> ( 0 ~ 1 )

private:
	//멀티 스레드 로딩용 함수.
	static DWORD WINAPI BackgroundLoadingThread(LPVOID _param);
	void ProcessMainThreadTasks();
	void CreateDefaultLight();

private:
	//테스트용. 
	void CreateTestDecal();
	void ControlPlayerStatus();
	void CreateTestMesh();
	void CreateTestDummy();

	void LoadItemIcons();
	void LoadItemGradeMaterial();

private:
	shared_ptr<GameObject> m_CemeteryParent;

	//=====================카메라 관련 변수=====================//
	shared_ptr<BiancaCamera> m_cameraScript = nullptr;


	shared_ptr<Shader> m_defaultshader = nullptr;
	shared_ptr<Shader> m_testShader = nullptr;

	//=====================UI관련 변수=====================//
	shared_ptr<Cursor> m_cursor = nullptr;
	shared_ptr<GameObject> m_itemBox = nullptr;

	shared_ptr<GameObject> m_charStatPanel = nullptr;
	shared_ptr<GameObject> m_charEquipPanel = nullptr;
	shared_ptr<GameObject> m_charMainPanel = nullptr;
	shared_ptr<GameObject> m_charInventoryPanel = nullptr;

	shared_ptr<GameObject> m_timePanel = nullptr;
	shared_ptr<GameObject> m_dayPanel = nullptr;
	//=====================UI관련 변수=====================//

	// 테스트용
	shared_ptr<GameObject> m_navMesh;
	
	shared_ptr<Player> m_player;


	int m_selectedCharacterIdx = 0; //0 : 비앙카 , 1 : 니키

	float m_lastFloatTime = 0.f;
	int m_lastTime = -1;
	shared_ptr<D2DText> m_test;

private:
	CRITICAL_SECTION m_loadingCS;
	HANDLE m_loadingThread;
	atomic<bool> m_loadingComplete{ false };

	//메인 스레드 작업 큐
	queue<function<void()>> m_mainThreadTasks;
	CRITICAL_SECTION m_mainThreadTasksCS;
	atomic<bool> m_objectsCreated{ false };

private:
	//테스트용 아이템
	// 인벤토리 슬롯 관련 추가
	vector<shared_ptr<ItemSlot>> m_inventorySlots;
	vector<shared_ptr<ItemSlot>> m_equipmentSlots;
	vector<shared_ptr<Item>> m_testItems; // 테스트용 아이템들

	shared_ptr<InventoryManager> m_inventoryManager;

public:
	// 새로운 함수들 추가
	void CreateInventorySlots();
	void CreateEquipmentSlots();
	void CreateTestItems();
	// 함수 추가
	void CreateInventoryManager();
};



