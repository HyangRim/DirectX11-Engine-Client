#pragma once

class Sky;
class Camera;
class BaseCollider;
#include "QuadTree.h"
#include "LooseQuadTree.h"
#include "CollisionBenchmark.h"
#include "SceneObjectManager.h"

// 충돌 broadphase 방식 (F11 로 전환, F2 로 비교 벤치마크 기록)
enum class CollisionMode : uint8 {
    ScreenQuadTree = 0,     // 기존 : 화면 좌표 QuadTree (화면 안 객체만)
    LooseQuadTree = 1,      // 새 방식 : 월드 XZ Loose QuadTree (모든 충돌체)
};

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

    // UI 부모-자식 관계 등록
    void RegisterUIParent(shared_ptr<GameObject> parent);
    void RegisterUIChild(shared_ptr<GameObject> child);


    void SetSky(shared_ptr<Sky> _sky) { m_objectManager->SetSky(_sky); }

    unordered_set<shared_ptr<GameObject>>& GetObjects() { return m_objectManager->GetObjects(); }
    unordered_set<shared_ptr<GameObject>>& GetUIObjects() { return m_objectManager->GetUIObjects(); }
    vector<shared_ptr<GameObject>>& GetUIChildren() { return m_objectManager->GetUIChildren(); }
    vector<shared_ptr<GameObject>>& GetUIParent() { return m_objectManager->GetUIParent(); }

    shared_ptr<GameObject> GetMainCamera();
    shared_ptr<GameObject> GetUICamera();
    shared_ptr<GameObject> GetLight() { return m_objectManager->GetLight(); }

    unique_ptr<SceneObjectManager>& GetObjectManager() { return m_objectManager; }


    bool IsDestroying() { return m_isDestroying; }
    void SetDestroying(bool destroying) { m_isDestroying = destroying; }

    void CheckCollision();
    void CheckCollisionWithQuadTree();

    // ---- 충돌 broadphase 선택 · 비교
    void SetCollisionMode(CollisionMode _mode);
    CollisionMode GetCollisionMode() const { return m_collisionMode; }
    LooseQuadTree* GetLooseQuadTree() { return m_looseTree.get(); }
    CollisionBenchmark* GetCollisionBenchmark() { return m_collisionBench.get(); }
    void StartCollisionBenchmark(int _frames = 600);

    // Picking 후보 : 화면 모드면 화면 트리 Query, Loose 모드면 월드 트리 레이 질의 (+ 화면 안 객체만)
    vector<shared_ptr<GameObject>> QueryRayCandidates(const Ray& _ray, shared_ptr<Camera> _camera);
    // 안개(FOW) 대상 : 화면 모드면 화면 트리에 넣은 객체, Loose 모드면 충돌체가 있는 활성 객체 전부
    void GetFOWObjects(vector<shared_ptr<GameObject>>& _out);

public:
    QuadTree* GetQuadTree() { return m_objectManager->GetQuadTree(); }
    weak_ptr<GameObject> GetPickedObj() { return m_objectManager->GetPickedObj(); }


private:
    //충돌 관련 HashTable
    //충돌체 간의 이전 프레임 충돌. 
    unordered_map<ULONG64, bool> m_mapColInfo;

    //그룹간의 충돌 체크? 일단 보류. 

protected:
    shared_ptr<GameObject> m_pickedObject;
    shared_ptr<GameObject> m_player;

private:
    unique_ptr<SceneObjectManager> m_objectManager;

    // ---- 충돌 broadphase
    void RunCollision();
    void HandleCollisionDebugKeys();
    void GatherColliders(vector<shared_ptr<BaseCollider>>& _out);
    void CheckCollisionWithLooseQuadTree(const vector<LooseQuadTree::Hit>* _precomputed = nullptr);
    void ResetCollisionStates();
    void MeasureBroadphases(CollisionBenchRow& _row, vector<ULONG64>& _truth, vector<LooseQuadTree::Hit>& _looseHits);
    int CountDriverStale(const vector<ULONG64>& _truth) const;

    struct LoosePairState {
        weak_ptr<BaseCollider> a;
        weak_ptr<BaseCollider> b;
        uint32 frame = 0;
    };
    CollisionMode m_collisionMode = CollisionMode::LooseQuadTree;
    unique_ptr<LooseQuadTree> m_looseTree;
    unordered_map<ULONG64, LoosePairState> m_loosePairs;   // Loose 방식에서 지금 닿아 있는 쌍
    uint32 m_collisionFrame = 0;
    vector<shared_ptr<BaseCollider>> m_colliderScratch;
    unique_ptr<CollisionBenchmark> m_collisionBench;
    int m_benchFrame = 0;
    double m_lastScreenUpdateUs = 0.0;     // 이번 프레임 UpdateQuadTree 시간
    int m_lastScreenNodesCreated = 0;      // 이번 프레임 UpdateQuadTree 가 만든 노드 수
    bool m_isDestroying = false;  // 소멸 중인지 확인하는 플래그
};