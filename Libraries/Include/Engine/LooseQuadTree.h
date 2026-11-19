#pragma once

class BaseCollider;

// =============================================================================
// LooseQuadTree : 월드 XZ 평면 기준 충돌 broadphase
//  - 레벨 L 의 칸 크기 = worldSize / 2^L. 레벨마다 칸 배열을 미리 만들어 둔다 (프레임마다 new / delete 없음)
//  - 객체는 '지름 <= 칸 크기' 인 가장 깊은 레벨에서, 중심이 들어 있는 칸 하나에만 들어간다
//  - 칸의 느슨한 경계 = 칸 + 사방 칸 절반 → 객체 몸은 반드시 그 안에 있다
//  - 쌍 찾기 : 자기 레벨(상대 인덱스가 클 때만) + 위(칸이 큰) 레벨 전부를 본다.
//             레벨마다 내 XZ 상자를 칸 절반만큼 넓혀서 겹치는 칸만 꺼낸다 → 같은 쌍은 정확히 한 번
//  - 맵 밖 / 맵보다 큰 객체는 outside 목록에 두고 모든 객체와 검사한다
// =============================================================================
class LooseQuadTree
{
public:
    struct Hit
    {
        shared_ptr<BaseCollider> a;     // ID 가 작은 쪽
        shared_ptr<BaseCollider> b;
        ULONG64 key = 0;                // COLLIDER_ID 와 같은 규칙 (left = 작은 ID)
    };

    struct Stats
    {
        int items = 0;          // 트리에 있는 충돌체 수
        int outside = 0;        // outside 목록 수
        int inserted = 0;       // 이번 Sync 에서 새로 들어온 수
        int removed = 0;        // 이번 Sync 에서 빠진 수
        int moved = 0;          // 칸 / 레벨이 바뀌어 옮긴 수
        int refits = 0;         // 월드 범위를 다시 잡은 횟수 (누적)
        long long aabbTests = 0;    // XZ 상자 겹침 검사 수
        long long narrowTests = 0;  // Intersects() 호출 수
        long long hits = 0;         // 실제로 닿은 쌍 수
        double narrowUs = 0.0;      // Intersects() 에 쓴 시간 (프로파일러가 켜져 있을 때만 잰다)
        double syncUs = 0.0;
        double queryUs = 0.0;
    };

    explicit LooseQuadTree(int maxDepth = 6);
    ~LooseQuadTree();

    void Clear();
    bool Empty() const { return m_lookup.empty(); }

    // 월드 범위 (정사각형). 지정하지 않으면 첫 Sync 때 충돌체 위치로 잡는다
    void SetWorldBounds(float minX, float minZ, float size);

    // 이번 프레임 충돌체 목록과 맞춘다 : 새로 들어온 것 넣기, 움직인 것 옮기기, 사라진 것 빼기
    void Sync(const vector<shared_ptr<BaseCollider>>& colliders);

    // 닿은 쌍만 out 에 담는다 (out 은 비우고 시작)
    void FindPairs(vector<Hit>& out);

    // 레이의 XZ 투영 선분 (origin ~ origin + dir * maxDist) 과 XZ 상자가 겹치는 충돌체 (Picking 후보)
    void QueryRay(const Vec3& origin, const Vec3& dir, float maxDist, vector<shared_ptr<BaseCollider>>& out) const;

    const Stats& GetStats() const { return m_stats; }
    float GetWorldMinX() const { return m_minX; }
    float GetWorldMinZ() const { return m_minZ; }
    float GetWorldSize() const { return m_size; }
    int GetMaxDepth() const { return m_maxDepth; }

    static ULONG64 MakeKey(uint32 idA, uint32 idB);

private:
    struct Item
    {
        shared_ptr<BaseCollider> col;
        uint32 id = 0;
        float minX = 0.f, minZ = 0.f, maxX = 0.f, maxZ = 0.f;
        int level = -1;         // -1 = outside
        int cell = -1;
        int slot = -1;          // 칸 목록 (또는 outside 목록) 안의 위치
        uint32 seen = 0;
        bool alive = false;
    };

    static bool ComputeXZBounds(BaseCollider* c, float& minX, float& minZ, float& maxX, float& maxZ);
    bool Locate(const Item& it, int& level, int& cell) const;
    void Place(int idx);
    void Unplace(int idx);
    void ResetGrid(float minX, float minZ, float size);
    void FitBounds(const vector<int>& indices);
    void TestPair(int ia, int ib, vector<Hit>& out);

private:
    int m_maxDepth;
    float m_minX = 0.f, m_minZ = 0.f, m_size = 0.f;
    bool m_hasBounds = false;

    vector<float> m_cellSize;               // 레벨별 칸 크기
    vector<int> m_dim;                      // 레벨별 한 변 칸 수 (2^L)
    vector<vector<vector<int>>> m_cells;    // [레벨][칸] → 아이템 인덱스
    vector<int> m_levelCount;               // 레벨별 아이템 수 (빈 레벨 건너뛰기)
    vector<int> m_outside;

    vector<Item> m_items;
    vector<int> m_free;
    unordered_map<BaseCollider*, int> m_lookup;
    uint32 m_frame = 0;

    Stats m_stats;
};
