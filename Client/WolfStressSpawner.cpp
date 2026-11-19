#include "pch.h"
#include "WolfStressSpawner.h"
#include "NavMesh.h"
#include "Wolf.h"
#include "HealthBar.h"
#include "BaseCollider.h"
#include "Player.h"
#include <algorithm>
#include <chrono>

namespace
{
    // 늑대는 같은 모델이라 한 번에 인스턴싱으로 그린다. 엔진 · 셰이더의 한 묶음 한도가 250 (MAX_MODEL_INSTANCE)
    // → 원래 배치(늑대 6) 를 더해도 넘지 않게 200 까지만
    const int STAGES[] = { 0, 50, 100, 200 };
    constexpr int MAX_STRESS_WOLVES = MAX_MODEL_INSTANCE - 50;
    constexpr uint32 BASE_SEED = 20261120u;
}

// ------------------------------------------------------------------ NavPointSampler
NavPointSampler::NavPointSampler(const NavMesh& _nav)
{
    const int n = _nav.GetTriangleCount();
    float total = 0.f;
    for (int i = 0; i < n; ++i)
    {
        const NavMeshTriangle& t = _nav.GetTriangle(i);
        const float area = 0.5f * (t.vertices[1] - t.vertices[0]).Cross(t.vertices[2] - t.vertices[0]).Length();
        if (area <= 1e-6f) continue;
        total += area;
        m_a.push_back(t.vertices[0]); m_b.push_back(t.vertices[1]); m_c.push_back(t.vertices[2]);
        m_cdf.push_back(total);
    }
}

Vec3 NavPointSampler::Sample(std::mt19937& _rng) const
{
    if (m_cdf.empty()) return Vec3::Zero;
    std::uniform_real_distribution<float> U(0.f, 1.f);
    const float r = U(_rng) * m_cdf.back();
    size_t i = static_cast<size_t>(std::lower_bound(m_cdf.begin(), m_cdf.end(), r) - m_cdf.begin());
    if (i >= m_cdf.size()) i = m_cdf.size() - 1;
    float u = U(_rng), v = U(_rng);
    if (u + v > 1.f) { u = 1.f - u; v = 1.f - v; }   // 삼각형 안 균일 분포
    return m_a[i] + (m_b[i] - m_a[i]) * u + (m_c[i] - m_a[i]) * v;
}

Vec3 NavPointSampler::SampleNear(std::mt19937& _rng, const Vec3& _center, float _radius) const
{
    Vec3 p = Sample(_rng);
    for (int k = 0; k < 256; ++k)
    {
        const float dx = p.x - _center.x, dz = p.z - _center.z;
        if (dx * dx + dz * dz <= _radius * _radius) break;
        p = Sample(_rng);
    }
    return p;
}

// ------------------------------------------------------------------ WolfStressSpawner
WolfStressSpawner::WolfStressSpawner(shared_ptr<NavMesh> _nav, CreateFn _create)
    : m_nav(std::move(_nav)), m_create(std::move(_create))
{
    if (m_nav) m_sampler = make_unique<NavPointSampler>(*m_nav);
}

WolfStressSpawner::~WolfStressSpawner() = default;

void WolfStressSpawner::Cycle()
{
    m_stage = (m_stage + 1) % static_cast<int>(std::size(STAGES));
    Spawn(STAGES[m_stage]);
}

void WolfStressSpawner::Clear()
{
    // 늑대 + 늑대가 만든 부속 오브젝트 (충돌체 디버그 구 · 체력바 UI 패널) 를 같이 지운다
    for (shared_ptr<Wolf>& wolf : m_wolves)
    {
        if (!wolf) continue;
        if (shared_ptr<BaseCollider> col = wolf->GetCollider())
            if (shared_ptr<GameObject> dbg = col->GetDebugObject())
                CURSCENE->Remove(dbg);
        if (shared_ptr<HealthBar> bar = wolf->GetHealthBar())
            if (shared_ptr<GameObject> panel = bar->GetPanelObject())
                CURSCENE->Remove(panel);
        CURSCENE->Remove(wolf);
    }
    m_wolves.clear();
}

void WolfStressSpawner::Spawn(int _count)
{
    SpawnImpl(_count, nullptr, 0.f);
}

void WolfStressSpawner::SpawnNear(int _count, const Vec3& _center, float _radius)
{
    SpawnImpl(_count, &_center, _radius);
}

void WolfStressSpawner::SetTargetAll(shared_ptr<Player> _target)
{
    for (shared_ptr<Wolf>& wolf : m_wolves)
        if (wolf) wolf->SetTarget(_target);
}

void WolfStressSpawner::SpawnImpl(int _count, const Vec3* _center, float _radius)
{
    Clear();
    if (_count <= 0 || !m_sampler || m_sampler->Empty() || !m_create)
    {
        wprintf(L"[Stress] wolves : 0\n");
        return;
    }

    const auto t0 = std::chrono::high_resolution_clock::now();
    std::mt19937 rng(BASE_SEED + static_cast<uint32>(_count));
    std::uniform_real_distribution<float> yaw(0.f, 360.f);

    _count = min(_count, MAX_STRESS_WOLVES);
    m_wolves.reserve(_count);
    for (int i = 0; i < _count; ++i)
    {
        const Vec3 pos = _center ? m_sampler->SampleNear(rng, *_center, _radius) : m_sampler->Sample(rng);
        shared_ptr<Wolf> wolf = m_create(pos, Vec3(0.f, yaw(rng), 0.f));
        if (!wolf) continue;
        // 씬 Start 이후에 들어온 객체는 Start 가 자동으로 불리지 않으므로 직접 부른다
        wolf->Start();
        m_wolves.push_back(wolf);
    }

    const double ms = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t0).count();
    TIME->ResetDeltaTime();   // 배치하느라 걸린 시간이 다음 프레임 DT 로 들어가지 않게
    wprintf(L"[Stress] wolves : %d (spawn %.0f ms)\n", static_cast<int>(m_wolves.size()), ms);
}
