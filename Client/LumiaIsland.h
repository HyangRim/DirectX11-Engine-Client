#pragma once
#include "Engine/Scene.h"
#include "IExecute.h"

#include "InventoryManager.h"
#include "UIManager.h"
#include "ItemSlot.h"

class GameObject;
class Player;
class Cursor;
class BiancaCamera;
class Wolf;
class Alpha;

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
	void CreateDefaultLight();
	void SelectCharacter();
	void CreateAndSetUIManager();

	//땅, 인테리어, 환경 생성. 
	void CreateCemeteryBase();
	void CreateCemeteryInterior();
	void CreateCemeterySmallInterior();
	void CreateCemeteryEnvironment();
	void CreateCemeteryItemBox();
	// NavMesh 관련 추가
	void CreateNavMesh();
	void CreateCharacterNicky();
	void CreateCharacterBianca();

	shared_ptr<Wolf> CreateMonsterWolf(Vec3 _pos, Vec3 _rot = Vec3(0,0,0));
	shared_ptr<Alpha> CreateMonsterAlpha(Vec3 _pos, Vec3 _rot = Vec3(0,0,0));

	//=====================UI관련 함수=====================//
	void CreateCursor();

	void LoadItemBoxImages();
	void CreateItemBoxPanel();
	void CheckPickedItemBox();

	void OnItemBoxSlotClicked(int _slotIndex, SLOTTYPE _slotType);
	void UpdateItemBoxSlots(shared_ptr<GameObject> _itemBoxObject);
	//=====================UI관련 함수=====================//

	//=====================제작 완료 확인 함수=======================//
	bool IsCraftStateCompleted(); // 새로 추가
	//=====================제작 완료 확인 함수=======================//

	//=====================스킬 완료 확인 함수=======================//
	bool IsQSkillCompleted(); // 새로 추가
	bool IsWSkillCompleted();
	bool IsESkillCompleted();
	bool IsRSkillCompleted();
	bool IsCounterSkillCompleted();
	//=====================스킬 완료 확인 함수=======================//

	//=====================스킬 레벨업 함수=======================//
	void HandleSkillLevelUpInput();
	void LevelUpSkill(int skillIndex);

	// NavMesh 경로 진단 (F8로 켜고 끄기)
	void UpdateNavPathDebug();
	void DrawNavPathDebug();
	bool m_navPathDebug = true;
	//=====================스킬 레벨업 함수=======================//

private:
	//멀티 스레드 로딩용 함수.
	static DWORD WINAPI BackgroundLoadingThread(LPVOID _param);
	void ProcessMainThreadTasks();
	
private:
	//테스트용. 
	void CreateTestDecal();
	void ControlPlayerStatus();
	void CreateTestDummy();


private:
	shared_ptr<GameObject> m_CemeteryParent;

	//=====================카메라 관련 변수=====================//
	shared_ptr<BiancaCamera> m_cameraScript = nullptr;


	shared_ptr<Shader> m_defaultshader = nullptr;
	shared_ptr<Shader> m_testShader = nullptr;

	//=====================UI관련 변수=====================//
	shared_ptr<Cursor> m_cursor = nullptr;

	shared_ptr<GameObject> m_itemBox = nullptr;
	shared_ptr<GameObject> m_currentItemBox = nullptr;
	vector<shared_ptr<ItemSlot>> m_itemBoxSlots;


	//=====================UI관련 변수=====================//

	// 테스트용
	shared_ptr<GameObject> m_navMesh;

	// 충돌 벤치마크용 늑대 대량 배치 (F3 : 0 → 50 → 100 → 200 → 0)
	unique_ptr<class WolfStressSpawner> m_wolfSpawner;
	void HandleWolfStressInput();

	// 프레임 프로파일러 : Shift + F2 로 600 프레임, 또는 명령줄 자동 측정
	//   Client.exe --autoprofile [--wolves=N] [--near=R] [--aggro] [--frames=M] [--warmup=K] [--warmupsec=S]
	//              [--collisionbench=N] [--mute] [--exit] [--collision=loose|screen]
	struct AutoProfile
	{
		bool enabled = false;
		int wolves = 0;
		float nearRadius = 0.f;   // 0 이면 맵 전체, 아니면 플레이어 반경 안
		bool aggro = false;
		int frames = 600;
		int warmup = 240;
		float warmupSec = 0.f;   // 0 보다 크면 늑대 배치 후 '게임 시간' 으로 기다린다 (프레임이 느린 방식과 같은 조건)
		int benchFrames = 0;     // 0 보다 크면 프로파일 대신 충돌 정확성 기록 (CollisionBenchmark)
		float timer = 0.f;
		bool walk = false;          // --walk : 플레이어가 고정 순찰 경로를 계속 돈다
		vector<Vec3> route;         // 시작 위치 주변 NavMesh 지점 (고정 시드)
		int routeIndex = 0;
		float legTimer = 0.f;       // 한 구간에 쓴 게임 시간 (막히면 다음 지점으로)
		bool mute = false;
		bool exitAfter = false;
		int stage = 0;
		int counter = 0;
	} m_autoProfile;
	void InitAutoProfile();
	void UpdateProfilerInput();
	string MakeProfileNote();
	void UpdateAutoWalk();
	
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
	shared_ptr<UIManager> m_uiManager;
};



