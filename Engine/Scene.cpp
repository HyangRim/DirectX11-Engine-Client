#include "pch.h"
#include "Scene.h"
#include <iostream>
#include "GameObject.h"
#include "BaseCollider.h"
#include "Camera.h"
#include "Button.h"
#include "Sky.h"
#include "Light.h"
#include "Terrain.h"
#include "QuadTree.h"
#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "UIPanel.h"
#include "SceneObjectManager.h"
#include "LooseQuadTree.h"
#include "CollisionBenchmark.h"
#include "FrameProfiler.h"
#include <algorithm>
#include <chrono>

Scene::Scene()
{
    m_objectManager = make_unique<SceneObjectManager>();
    m_looseTree = make_unique<LooseQuadTree>(6);
    m_collisionBench = make_unique<CollisionBenchmark>();
}

Scene::~Scene()
{
   
}

void Scene::RegisterUIParent(shared_ptr<GameObject> parent)
{
    m_objectManager->RegisterUIParent(parent);
}

void Scene::RegisterUIChild(shared_ptr<GameObject> child)
{
    m_objectManager->RegisterUIChild(child);
}

void Scene::Start()
{
    //충돌 판정 초기화. 
    m_mapColInfo.clear();
    m_objectManager->Start();
    m_objectManager->UpdateQuadTree();
}

void Scene::Update()
{
    { PROFILE_SCOPE("Update", "Objects(all)"); m_objectManager->Update(); }

    {
        PROFILE_SCOPE("Update", "ScreenQuadTree");
        // 화면 트리는 화면 모드에서만 매 프레임 다시 짓는다.
        // Loose 모드는 Picking · FOW 가 화면 트리를 쓰지 않으므로 만들어만 둔다 (비교 기록 중일 때만 측정용으로 짓는다)
        const bool buildScreenTree = (m_collisionMode == CollisionMode::ScreenQuadTree) || (m_collisionBench && m_collisionBench->IsRecording());
        const int nodesBefore = QuadTree::GetCreatedNodeCount();
        const auto t0 = std::chrono::high_resolution_clock::now();
        if (buildScreenTree) m_objectManager->UpdateQuadTree();
        else m_objectManager->EnsureQuadTree();
        m_lastScreenUpdateUs = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - t0).count();
        m_lastScreenNodesCreated = QuadTree::GetCreatedNodeCount() - nodesBefore;
    }

    HandleCollisionDebugKeys();

    if(INPUT->GetButtonDown(KEY_TYPE::LBUTTON) != false) {
        PROFILE_SCOPE("Update", "Pick");
        m_pickedObject = m_objectManager->PickObjectOrUI(); 
    }
    //이 밑에다가 디버그용 
#if _DEBUG
    GUI->ShowPickedObj();
#endif
}

void Scene::FixedUpdate()
{
    PROFILE_SCOPE("Fixed", "Objects(all)");
    m_objectManager->FixedUpdate();
}

void Scene::LateUpdate()
{

    // 충돌 : 선택한 broadphase 로 처리 (기록 중이면 세 방식을 같은 상태로 비교)
    { PROFILE_SCOPE("Late", "Collision"); RunCollision(); }


    { PROFILE_SCOPE("Late", "Objects(all)"); m_objectManager->LateUpdate(); }

    // start = std::chrono::high_resolution_clock::now();

    //auto end = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    //cout << "BruteForce 걸리는 시간 :" << duration.count() << "us\n";

    // 지연 삭제 처리 추가
    { PROFILE_SCOPE("Late", "PendingDestroy"); m_objectManager->ProcessPendingDestroy(); }
}

void Scene::Render()
{
    /*const auto& cameras = m_objectManager->GetCameras();
    for (auto camera : cameras) {

        Camera* cam = camera->GetCamera().get();
        if (cam->GetProjectionType() == ProjectionType::Perspective) {
            RenderGameCamera(cam);
        }
        else {
            RenderUICamera(cam);
        }
    }*/

    auto main_camera = m_objectManager->GetMainCamera();
    Camera* cam = main_camera->GetCamera().get();
    RenderGameCamera(cam);

    auto ui_Camera = m_objectManager->GetUICamera();
    cam = ui_Camera->GetCamera().get();
    RenderUICamera(cam);
}

void Scene::RenderGameCamera(Camera* cam)
{
    GRAPHICS->ClearShadowDepthStencilView();
    GRAPHICS->SetShadowDepthStencilView();

    Light* light = GetLight()->GetLight().get();

    cam->SetStaticData();
    { PROFILE_SCOPE("Render", "Camera.SortGameObject"); cam->SortGameObject(); }

    if (light) {
        PROFILE_SCOPE("Render", "ShadowPass");
        light->SetVPMatrix(cam, 200.0f, ::XMMatrixOrthographicLH(300.f, 300.f, 1.f, 500.f));
        cam->Render_Forward(true);
        Viewport& vp = GRAPHICS->GetShadowViewport();
        cam->Render_Backward(true);
    }

    GRAPHICS->SetRTVAndDSV();
    //GRAPHICS->ClearDepthStencilView(); // 이 줄 추가

    cam->SetStaticData();


    //if (GetQuadTree() == nullptr) return;
    //if (GetQuadTree()->GetInsertedObject().empty()) return;
    //vector<shared_ptr<GameObject>> objects(GetQuadTree()->GetInsertedObject().begin(), GetQuadTree()->GetInsertedObject().end());
    static vector<shared_ptr<GameObject>> combined;
    combined.clear();
    vector<shared_ptr<GameObject>> forward = cam->GetForwardObjects();
    vector<shared_ptr<GameObject>> backward = cam->GetBackwardObjects();
    combined.reserve(forward.size() + backward.size());

    if (m_objectManager->m_sky)
        m_objectManager->m_sky->Render(cam);
    combined.insert(combined.end(), forward.begin(), forward.end());
    combined.insert(combined.end(), backward.begin(), backward.end());


    
    { PROFILE_SCOPE("Render", "MainPass"); RENDER->Render(combined, false); }
    //RENDER->Render(cam->GetBackwardObjects(), false);
    /*cam->Render_Forward(false);
    if (m_objectManager->m_sky)
        m_objectManager->m_sky->Render(cam);
    cam->Render_Backward(false);*/
}

void Scene::RenderUICamera(Camera* cam)
{
    PROFILE_SCOPE("Render", "UICamera");
    GRAPHICS->ClearDepthStencilView();

    cam->SetStaticData();
    cam->SortGameObject();
    cam->Render_Forward(false);
    cam->Render_Backward(false);
}

void Scene::Add(shared_ptr<GameObject> _object)
{
    m_objectManager->Add(_object);
}

void Scene::AddUIObject(shared_ptr<GameObject> _object, bool isParent)
{
    m_objectManager->AddUIObject(_object, isParent);
}

void Scene::Remove(shared_ptr<GameObject> _object)
{
    // UI 객체의 경우 계층적 삭제 사용
    if (m_objectManager->GetUIObjects().find(_object) != m_objectManager->GetUIObjects().end()) {
        m_objectManager->MarkUIObjectForDestroyWithChildren(_object);
    }
    else {
        m_objectManager->Remove(_object);
    }
}

shared_ptr<GameObject> Scene::GetMainCamera()
{
    return m_objectManager->GetMainCamera();
}

shared_ptr<GameObject> Scene::GetUICamera()
{
    return m_objectManager->GetUICamera();
}


//포폴 차이점 용으로 남겨두기. 
void Scene::CheckCollision()
{
    //1. m_gameObjects끼리 for문 돌려서 검사한다. 
    //2. Collider만들 때, obj를 받는다. 
    vector<shared_ptr<BaseCollider>> colliders;
    const auto& objects = m_objectManager->GetObjects();
    for (auto& object : objects) {
        if (object->GetCollider() == nullptr)
            continue;

        colliders.push_back(object->GetCollider());
    }

    //BruteForce
    //쿼드 트리 같은 거. 

    //Collider끼리 검사. -> GameObject에 Collision처리 해야하는데. 
    for (uint32 idx = 0; idx < colliders.size(); ++idx) {
        if (colliders[idx].get()->GetActive() == false)
            continue;

        for (uint32 jdx = idx + 1; jdx < colliders.size(); ++jdx) {
            shared_ptr<BaseCollider>& other = colliders[jdx];

            if (other.get()->GetActive() == false)
                continue;


            COLLIDER_ID id;
            id.left_id = colliders[idx].get()->GetID();
            id.right_id = colliders[jdx].get()->GetID();

            auto colliderMapIter = m_mapColInfo.find(id.ID);

            //충돌 정보가 미 등록 상태일 경우.(충돌하지 않았다로 입력.) 
            if (colliderMapIter == m_mapColInfo.end()) {
                m_mapColInfo.insert(make_pair(id.ID, false));
                colliderMapIter = m_mapColInfo.find(id.ID);
            }

            //현재 Collider끼리 충돌했을 경우에. 
            if (colliders[idx]->Intersects(other)) {

                if (colliderMapIter->second == false) {
                    //이번 프레임에 막 충돌한 경우.

                    //TODO : 이벤트 후처리 시스템 어떻게? 
                    colliders[idx].get()->GetGameObject()->OnCollisionEnter(colliders[jdx].get()->GetGameObject());
                    colliders[jdx].get()->GetGameObject()->OnCollisionEnter(colliders[idx].get()->GetGameObject());
                    colliderMapIter->second = true;
                }
                else {
                    //이전 프레임에도 충돌하고 있던 경우. 
                    colliders[idx].get()->GetGameObject()->OnCollision(colliders[jdx].get()->GetGameObject());
                    colliders[jdx].get()->GetGameObject()->OnCollision(colliders[idx].get()->GetGameObject());
                }
            }
            else {
                //충돌하지 않았을 경우. 
                if (colliderMapIter->second == true) {
                    //이전에 충돌하고 있었으면. 
                    colliders[idx].get()->GetGameObject()->OnCollisionExit(colliders[jdx].get()->GetGameObject());
                    colliders[jdx].get()->GetGameObject()->OnCollisionExit(colliders[idx].get()->GetGameObject());
                    colliderMapIter->second = false;
                }
            }
        }
    }
}

void Scene::CheckCollisionWithQuadTree()
{
    auto quadTree = m_objectManager->GetQuadTree();
    if (!quadTree) return;

    quadTree->CheckCollisionsInTree(GetMainCamera()->GetCamera(), m_mapColInfo);
}



// =============================================================================
// 충돌 broadphase : 기존 화면 QuadTree ↔ 월드 XZ Loose QuadTree
//  - F11 : 방식 전환 (전환할 때 닿아 있던 쌍은 Exit 를 보내고 상태를 비운다)
//  - F2  : 600 프레임 비교 기록 시작 / 취소 → ..\tmp\benchmark\collision_*.csv
// =============================================================================
namespace
{
    using CollisionClock = std::chrono::high_resolution_clock;
    inline double CollisionElapsedUs(CollisionClock::time_point t0)
    {
        return std::chrono::duration<double, std::micro>(CollisionClock::now() - t0).count();
    }

    void FireCollisionExit(const shared_ptr<BaseCollider>& a, const shared_ptr<BaseCollider>& b)
    {
        shared_ptr<GameObject> ga = a->GetGameObject();
        shared_ptr<GameObject> gb = b->GetGameObject();
        if (!ga || !gb) return;
        ga->OnCollisionExit(gb);
        gb->OnCollisionExit(ga);
    }

    // 정렬된 두 목록 비교 : a 에만 있는 개수
    int CountOnlyIn(const vector<ULONG64>& a, const vector<ULONG64>& b)
    {
        int n = 0;
        size_t j = 0;
        for (ULONG64 k : a)
        {
            while (j < b.size() && b[j] < k) ++j;
            if (j >= b.size() || b[j] != k) ++n;
        }
        return n;
    }

    const wchar_t* CollisionModeName(CollisionMode m)
    {
        return m == CollisionMode::ScreenQuadTree ? L"ScreenQuadTree" : L"LooseQuadTree";
    }
}

void Scene::SetCollisionMode(CollisionMode _mode)
{
    if (_mode == m_collisionMode) return;
    ResetCollisionStates();
    m_collisionMode = _mode;
    if (_mode == CollisionMode::ScreenQuadTree && m_looseTree)
        m_looseTree->Clear();
    // 화면 트리를 비워 둔다 (Loose 모드에선 안 쓰고, 화면 모드로 돌아오면 다시 지어진다)
    m_objectManager->ClearQuadTree();
    wprintf(L"[Collision] mode : %s\n", CollisionModeName(m_collisionMode));
}

vector<shared_ptr<GameObject>> Scene::QueryRayCandidates(const Ray& _ray, shared_ptr<Camera> _camera)
{
    QuadTree* qt = GetQuadTree();
    if (m_collisionMode == CollisionMode::ScreenQuadTree || !m_looseTree)
        return qt ? qt->Query(_ray, _camera) : vector<shared_ptr<GameObject>>{};

    vector<shared_ptr<BaseCollider>> cols;
    m_looseTree->QueryRay(_ray.position, _ray.direction, 1000.f, cols);
    vector<shared_ptr<GameObject>> out;
    out.reserve(cols.size());
    for (const shared_ptr<BaseCollider>& c : cols)
    {
        shared_ptr<GameObject> go = c->GetGameObject();
        if (!go) continue;
        if (qt && _camera && !qt->IsObjectVisible(go, _camera)) continue;   // 기존 Query 와 같게 화면 안만
        out.push_back(go);
    }
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

void Scene::GetFOWObjects(vector<shared_ptr<GameObject>>& _out)
{
    _out.clear();
    if (m_collisionMode == CollisionMode::ScreenQuadTree)
    {
        if (QuadTree* qt = GetQuadTree())
            _out.assign(qt->GetInsertedObject().begin(), qt->GetInsertedObject().end());
        return;
    }
    for (const auto& object : m_objectManager->GetObjects())
        if (object && object->GetActive() && object->GetCollider())
            _out.push_back(object);
}

void Scene::StartCollisionBenchmark(int _frames)
{
    if (!m_collisionBench) return;
    m_benchFrame = 0;
    m_collisionBench->Start(_frames, static_cast<int>(m_collisionMode));
}

void Scene::HandleCollisionDebugKeys()
{
    if (INPUT->GetButtonDown(KEY_TYPE::F11))
    {
        SetCollisionMode(m_collisionMode == CollisionMode::ScreenQuadTree ? CollisionMode::LooseQuadTree : CollisionMode::ScreenQuadTree);
    }
    // Shift + F2 는 프레임 프로파일러 (LumiaIsland) 라서 여기서는 건너뛴다
    if (INPUT->GetButtonDown(KEY_TYPE::F2) && !INPUT->GetButton(KEY_TYPE::LSHIFT) && m_collisionBench)
    {
        if (m_collisionBench->IsRecording())
        {
            m_collisionBench->Cancel();
            wprintf(L"[CollisionBench] canceled\n");
        }
        else
        {
            StartCollisionBenchmark(600);
        }
    }
}

void Scene::GatherColliders(vector<shared_ptr<BaseCollider>>& _out)
{
    _out.clear();
    for (const auto& object : m_objectManager->GetObjects())
    {
        if (!object || !object->GetActive()) continue;
        shared_ptr<BaseCollider> col = object->GetCollider();
        if (!col || !col->GetActive()) continue;
        _out.push_back(col);
    }
}

void Scene::RunCollision()
{
    const bool recording = m_collisionBench && m_collisionBench->IsRecording();

    CollisionBenchRow row;
    vector<ULONG64> truth;
    vector<LooseQuadTree::Hit> looseHits;
    if (recording)
        MeasureBroadphases(row, truth, looseHits);

    if (m_collisionMode == CollisionMode::ScreenQuadTree)
    {
        CheckCollisionWithQuadTree();
        // 안 쓰는 동안 Loose 트리가 충돌체 shared_ptr 를 붙잡고 있지 않게 비운다
        if (!recording && m_looseTree && !m_looseTree->Empty())
            m_looseTree->Clear();
    }
    else
    {
        // 기록 중이면 방금 잰 결과를 그대로 써서 이벤트를 낸다 (같은 일을 두 번 하지 않게)
        CheckCollisionWithLooseQuadTree(recording ? &looseHits : nullptr);
    }

    if (recording)
    {
        row.driver = static_cast<int>(m_collisionMode);
        row.driverStale = CountDriverStale(truth);
        m_collisionBench->AddRow(row);
    }
}

void Scene::CheckCollisionWithLooseQuadTree(const vector<LooseQuadTree::Hit>* _precomputed)
{
    if (!m_looseTree) return;

    vector<LooseQuadTree::Hit> local;
    const vector<LooseQuadTree::Hit>* hits = _precomputed;
    if (!hits)
    {
        { PROFILE_SCOPE("Collision", "GatherColliders"); GatherColliders(m_colliderScratch); }
        { PROFILE_SCOPE("Collision", "LooseSync"); m_looseTree->Sync(m_colliderScratch); }
        { PROFILE_SCOPE("Collision", "LooseFindPairs(+Narrow)"); m_looseTree->FindPairs(local); }
        if (FrameProfiler::s_active)
        {
            const LooseQuadTree::Stats& st = m_looseTree->GetStats();
            FrameProfiler::Get().Add("Collision", "Narrow(Intersects)", st.narrowUs, static_cast<int>(st.narrowTests));
            FrameProfiler::Get().Add("Collision", "AabbTests(count)", 0.0, static_cast<int>(st.aabbTests));
            FrameProfiler::Get().Add("Collision", "Hits(count)", 0.0, static_cast<int>(st.hits));
        }
        m_colliderScratch.clear();
        hits = &local;
    }

    ++m_collisionFrame;

    // 닿은 쌍 : 처음이면 Enter, 이어지면 Stay
    {
    PROFILE_SCOPE("Collision", "Events(Enter/Stay)");
    for (const LooseQuadTree::Hit& h : *hits)
    {
        shared_ptr<GameObject> ga = h.a->GetGameObject();
        shared_ptr<GameObject> gb = h.b->GetGameObject();
        if (!ga || !gb) continue;

        auto it = m_loosePairs.find(h.key);
        if (it == m_loosePairs.end())
        {
            m_loosePairs.emplace(h.key, LoosePairState{ h.a, h.b, m_collisionFrame });
            ga->OnCollisionEnter(gb);
            gb->OnCollisionEnter(ga);
        }
        else
        {
            it->second.frame = m_collisionFrame;
            ga->OnCollision(gb);
            gb->OnCollision(ga);
        }
    }
    }

    PROFILE_SCOPE("Collision", "Events(Exit scan)");
    // 이번 프레임에 안 닿은 쌍 = 떨어짐 · 비활성 · 삭제 → Exit (화면 밖이어도 빠짐없이)
    for (auto it = m_loosePairs.begin(); it != m_loosePairs.end();)
    {
        if (it->second.frame == m_collisionFrame) { ++it; continue; }
        shared_ptr<BaseCollider> a = it->second.a.lock();
        shared_ptr<BaseCollider> b = it->second.b.lock();
        it = m_loosePairs.erase(it);
        if (a && b) FireCollisionExit(a, b);
    }
}

void Scene::ResetCollisionStates()
{
    unordered_map<uint32, shared_ptr<BaseCollider>> byId;
    for (const auto& object : m_objectManager->GetObjects())
    {
        if (!object) continue;
        if (shared_ptr<BaseCollider> c = object->GetCollider())
            byId[c->GetID()] = c;
    }

    // 기존 방식 : 닿아 있던 쌍에 Exit
    vector<pair<shared_ptr<BaseCollider>, shared_ptr<BaseCollider>>> exits;
    for (const auto& kv : m_mapColInfo)
    {
        if (!kv.second) continue;
        COLLIDER_ID id; id.ID = kv.first;
        auto a = byId.find(id.left_id), b = byId.find(id.right_id);
        if (a != byId.end() && b != byId.end()) exits.emplace_back(a->second, b->second);
    }
    m_mapColInfo.clear();

    // Loose 방식 : 닿아 있던 쌍에 Exit
    for (const auto& kv : m_loosePairs)
    {
        shared_ptr<BaseCollider> a = kv.second.a.lock(), b = kv.second.b.lock();
        if (a && b) exits.emplace_back(a, b);
    }
    m_loosePairs.clear();

    for (const auto& e : exits) FireCollisionExit(e.first, e.second);
}

void Scene::MeasureBroadphases(CollisionBenchRow& _row, vector<ULONG64>& _truth, vector<LooseQuadTree::Hit>& _looseHits)
{
    vector<shared_ptr<BaseCollider>> cols;
    GatherColliders(cols);
    _row.frame = m_benchFrame++;
    _row.eligible = static_cast<int>(cols.size());

    // ---- 1. 정답 : 전부 짝짓기 (ID 작은 쪽 기준으로 판정)
    {
        _truth.clear();
        const auto t0 = CollisionClock::now();
        long long narrow = 0;
        for (size_t i = 0; i < cols.size(); ++i)
        {
            for (size_t j = i + 1; j < cols.size(); ++j)
            {
                const bool iFirst = cols[i]->GetID() < cols[j]->GetID();
                shared_ptr<BaseCollider>& lo = iFirst ? cols[i] : cols[j];
                shared_ptr<BaseCollider>& hi = iFirst ? cols[j] : cols[i];
                ++narrow;
                if (lo->Intersects(hi))
                    _truth.push_back(LooseQuadTree::MakeKey(lo->GetID(), hi->GetID()));
            }
        }
        _row.bruteUs = CollisionElapsedUs(t0);
        _row.bruteNarrow = narrow;
        std::sort(_truth.begin(), _truth.end());
        _truth.erase(std::unique(_truth.begin(), _truth.end()), _truth.end());
        _row.brutePairs = static_cast<int>(_truth.size());
    }

    // ---- 2. 기존 : 화면 QuadTree (트리는 이번 프레임 Update 에서 이미 갱신됨)
    {
        _row.screenUpdateUs = m_lastScreenUpdateUs;
        _row.screenRebuilt = m_objectManager->WasQuadTreeRebuilt() ? 1 : 0;
        _row.screenNodesCreated = m_lastScreenNodesCreated;
        vector<ULONG64> pairs;
        if (QuadTree* qt = GetQuadTree())
        {
            _row.screenInserted = static_cast<int>(qt->GetInsertedObject().size());
            long long narrow = 0;
            const auto t0 = CollisionClock::now();
            qt->CollectCollisionPairs(pairs, narrow);
            _row.screenQueryUs = CollisionElapsedUs(t0);
            _row.screenNarrow = narrow;
        }
        std::sort(pairs.begin(), pairs.end());
        pairs.erase(std::unique(pairs.begin(), pairs.end()), pairs.end());
        _row.screenPairs = static_cast<int>(pairs.size());
        _row.screenMissed = CountOnlyIn(_truth, pairs);
        _row.screenExtra = CountOnlyIn(pairs, _truth);
    }

    // ---- 3. 새 방식 : 월드 XZ Loose QuadTree
    if (m_looseTree)
    {
        m_looseTree->Sync(cols);
        m_looseTree->FindPairs(_looseHits);
        const LooseQuadTree::Stats& s = m_looseTree->GetStats();
        _row.looseSyncUs = s.syncUs;
        _row.looseQueryUs = s.queryUs;
        _row.looseInserted = s.inserted;
        _row.looseRemoved = s.removed;
        _row.looseMoved = s.moved;
        _row.looseOutside = s.outside;
        _row.looseAabb = s.aabbTests;
        _row.looseNarrow = s.narrowTests;

        vector<ULONG64> pairs;
        pairs.reserve(_looseHits.size());
        for (const LooseQuadTree::Hit& h : _looseHits) pairs.push_back(h.key);
        std::sort(pairs.begin(), pairs.end());
        pairs.erase(std::unique(pairs.begin(), pairs.end()), pairs.end());
        _row.loosePairs = static_cast<int>(pairs.size());
        _row.looseMissed = CountOnlyIn(_truth, pairs);
        _row.looseExtra = CountOnlyIn(pairs, _truth);
    }
}

int Scene::CountDriverStale(const vector<ULONG64>& _truth) const
{
    // 이벤트를 낸 방식이 '닿아 있다' 고 들고 있는데 정답에는 없는 쌍 = Exit 가 안 나간 쌍
    int n = 0;
    if (m_collisionMode == CollisionMode::ScreenQuadTree)
    {
        for (const auto& kv : m_mapColInfo)
            if (kv.second && !std::binary_search(_truth.begin(), _truth.end(), kv.first)) ++n;
    }
    else
    {
        for (const auto& kv : m_loosePairs)
            if (!std::binary_search(_truth.begin(), _truth.end(), kv.first)) ++n;
    }
    return n;
}
