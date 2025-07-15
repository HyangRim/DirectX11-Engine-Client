#pragma once

class Sky;
class Camera;
class BaseCollider;
#include "QuadTree.h"

union COLLIDER_ID {
    struct {
        uint32 left_id;
        uint32 right_id;
    };
    ULONG64 ID;
};

class Scene
{
public:
    Scene();
    ~Scene();

public:
    virtual void Start();
    virtual void Update();
    virtual void FixedUpdate();
    virtual void LateUpdate();
    virtual void Render();

    void RenderGameCamera(Camera* cam);
    void RenderUICamera(Camera* cam);


    virtual void Add(shared_ptr<GameObject> _object);
    // UI 객체 추가 (새로 추가)
    void AddUIObject(shared_ptr<GameObject> _object, bool isParent = false);
    virtual void Remove(shared_ptr<GameObject> _object);

    // 지연 삭제 함수들 (새로 추가)
    void MarkForDestroy(shared_ptr<GameObject> obj);
    void MarkUIObjectForDestroy(shared_ptr<GameObject> obj);
    void ProcessPendingDestroy();

    unordered_set<shared_ptr<GameObject>>& GetObjects() { return m_gameObjects; }
    shared_ptr<GameObject> GetMainCamera();
    shared_ptr<GameObject> GetUICamera();
    shared_ptr<GameObject> GetLight() { return m_Lights.empty() ? nullptr : *m_Lights.begin(); }


    void PickUI();
    shared_ptr<class GameObject> Pick();
    shared_ptr<class GameObject> PickObjectOrUI();
    string ws2s(const wstring& wstr);

    void SetSky(shared_ptr<Sky> _sky) { m_sky = _sky; }

    void CheckCollision();
    void CheckCollisionWithQuadTree();

    ///////쿼드 트리 테스트용//////////////

private:
    unique_ptr<QuadTree> m_quadTree;
    bool m_quadTreeDirty = true;

public:
    void UpdateQuadTree();
    void GameObjectsTest();
    QuadTree* GetQuadTree() { return m_quadTree.get(); }
    Ray CreateRayFromScreen(const Vec2& screenPos, shared_ptr<Camera> camera);

    weak_ptr<GameObject> GetPickedObj() { return m_curPickedObj; }

    ///////////////////////////////////////


public:
    // UI 부모-자식 관계 등록
    void RegisterUIParent(shared_ptr<GameObject> parent);
    void RegisterUIChild(shared_ptr<GameObject> child);

private:
    void DestroyUIObjects();
    void DestroyNormalObjects();

    // 지연 삭제 처리 함수들
    void ProcessPendingNormalObjects();
    void ProcessPendingUIObjects();

private:
    //충돌 관련 HashTable
    //충돌체 간의 이전 프레임 충돌. 
    unordered_map<ULONG64, bool> m_mapColInfo;

    //그룹간의 충돌 체크? 일단 보류. 

public:
    unordered_set<shared_ptr<GameObject>> m_gameObjects;

    // 새로 추가: UI 객체들 분리 저장
    unordered_set<shared_ptr<GameObject>> m_uiObjects;    // UI 객체들
    vector<shared_ptr<GameObject>> m_uiParents;           // UI 부모들 (PanelUI 등)
    vector<shared_ptr<GameObject>> m_uiChildren;          // UI 자식들 (ImageUI 등)

    //Cache Camera;
    unordered_set<shared_ptr<GameObject>> m_cameras;
    //Cache Light;
    unordered_set<shared_ptr<GameObject>> m_Lights;

    // 지연 삭제 컨테이너들 (새로 추가)
    vector<shared_ptr<GameObject>> m_pendingDestroyNormal;     // 일반 객체 삭제 대기열
    vector<shared_ptr<GameObject>> m_pendingDestroyUI;         // UI 객체 삭제 대기열
    bool m_isDestroying = false;  // 소멸 중인지 확인하는 플래그


    shared_ptr<Sky> m_sky;
    weak_ptr<GameObject> m_curPickedObj;
};