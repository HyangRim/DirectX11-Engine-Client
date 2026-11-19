#pragma once
#include <random>
#include <functional>

class NavMesh;
class Wolf;

// =============================================================================
// 충돌 벤치마크용 늑대 대량 배치
//  - NavMesh 위 무작위 지점 (삼각형 넓이 비례) 에 진짜 늑대를 고정 시드로 깐다 → 같은 수면 매번 같은 배치
//  - 늑대는 평소와 똑같이 행동 트리 · NavMeshAgent · Rigidbody · 체력바를 가진다
//  - Cycle() : 0 → 50 → 100 → 200 → 0 (인스턴싱 한 묶음 한도 250 안에서)
// =============================================================================
class NavPointSampler
{
public:
    explicit NavPointSampler(const NavMesh& _nav);
    bool Empty() const { return m_cdf.empty(); }
    Vec3 Sample(std::mt19937& _rng) const;
    Vec3 SampleNear(std::mt19937& _rng, const Vec3& _center, float _radius) const;   // 반경 안 (못 찾으면 아무 곳)

private:
    vector<Vec3> m_a, m_b, m_c;
    vector<float> m_cdf;    // 넓이 누적 (마지막 값 = 전체 넓이)
};

class WolfStressSpawner
{
public:
    // _create : 위치 · 회전을 받아 늑대를 만들고 씬에 넣는 함수 (LumiaIsland::CreateMonsterWolf)
    using CreateFn = std::function<shared_ptr<Wolf>(Vec3, Vec3)>;

    WolfStressSpawner(shared_ptr<NavMesh> _nav, CreateFn _create);
    ~WolfStressSpawner();

    void Cycle();
    void Spawn(int _count);
    void SpawnNear(int _count, const Vec3& _center, float _radius);   // 중심 반경 안에만
    void SetTargetAll(shared_ptr<class Player> _target);            // 전부 추적 상태로 (피격 없이 어그로)
    void Clear();
    int GetCount() const { return static_cast<int>(m_wolves.size()); }

private:
    shared_ptr<NavMesh> m_nav;
    unique_ptr<NavPointSampler> m_sampler;
    CreateFn m_create;
    vector<shared_ptr<Wolf>> m_wolves;
    int m_stage = 0;
    void SpawnImpl(int _count, const Vec3* _center, float _radius);
};
