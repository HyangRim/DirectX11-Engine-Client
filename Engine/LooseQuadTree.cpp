#include "pch.h"
#include "LooseQuadTree.h"
#include "BaseCollider.h"
#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"
#include "FrameProfiler.h"
#include <chrono>
#include <cfloat>

namespace
{
    using Clock = std::chrono::high_resolution_clock;
    inline double ElapsedUs(Clock::time_point t0)
    {
        return std::chrono::duration<double, std::micro>(Clock::now() - t0).count();
    }

    constexpr float MIN_WORLD_SIZE = 64.f;
}

LooseQuadTree::LooseQuadTree(int maxDepth)
    : m_maxDepth(max(0, min(maxDepth, 10)))
{
}

LooseQuadTree::~LooseQuadTree() = default;

ULONG64 LooseQuadTree::MakeKey(uint32 idA, uint32 idB)
{
    // Scene 의 COLLIDER_ID 와 같은 배치 : 하위 32bit = 작은 ID, 상위 32bit = 큰 ID
    const uint32 lo = min(idA, idB), hi = max(idA, idB);
    return (static_cast<ULONG64>(hi) << 32) | static_cast<ULONG64>(lo);
}

void LooseQuadTree::Clear()
{
    for (auto& lv : m_cells)
        for (auto& c : lv) c.clear();
    std::fill(m_levelCount.begin(), m_levelCount.end(), 0);
    m_outside.clear();
    m_items.clear();
    m_free.clear();
    m_lookup.clear();
    m_hasBounds = false;
    m_stats = Stats{};
}

void LooseQuadTree::SetWorldBounds(float minX, float minZ, float size)
{
    // 이미 들어 있는 객체가 있으면 새 격자에 다시 넣는다
    vector<int> alive;
    for (int i = 0; i < static_cast<int>(m_items.size()); ++i)
        if (m_items[i].alive) { Unplace(i); alive.push_back(i); }
    ResetGrid(minX, minZ, size);
    for (int i : alive) Place(i);
}

void LooseQuadTree::ResetGrid(float minX, float minZ, float size)
{
    m_minX = minX; m_minZ = minZ; m_size = max(size, 1.f);
    m_hasBounds = true;
    const int levels = m_maxDepth + 1;
    m_cellSize.assign(levels, 0.f);
    m_dim.assign(levels, 0);
    m_levelCount.assign(levels, 0);
    if (static_cast<int>(m_cells.size()) != levels) m_cells.assign(levels, {});
    for (int L = 0; L < levels; ++L)
    {
        m_dim[L] = 1 << L;
        m_cellSize[L] = m_size / static_cast<float>(m_dim[L]);
        const size_t n = static_cast<size_t>(m_dim[L]) * m_dim[L];
        if (m_cells[L].size() != n) m_cells[L].assign(n, {});
        else for (auto& c : m_cells[L]) c.clear();
    }
    m_outside.clear();
}

bool LooseQuadTree::ComputeXZBounds(BaseCollider* c, float& minX, float& minZ, float& maxX, float& maxZ)
{
    switch (c->GetColliderType())
    {
    case ColliderType::Sphere:
    {
        const BoundingSphere& s = static_cast<SphereCollider*>(c)->GetBoundSphere();
        const float r = fabsf(s.Radius);
        minX = s.Center.x - r; maxX = s.Center.x + r;
        minZ = s.Center.z - r; maxZ = s.Center.z + r;
        return true;
    }
    case ColliderType::AABB:
    {
        const BoundingBox& b = static_cast<AABBBoxCollider*>(c)->GetBoundingBox();
        const float ex = fabsf(b.Extents.x), ez = fabsf(b.Extents.z);
        minX = b.Center.x - ex; maxX = b.Center.x + ex;
        minZ = b.Center.z - ez; maxZ = b.Center.z + ez;
        return true;
    }
    case ColliderType::OBB:
    {
        XMFLOAT3 corners[BoundingOrientedBox::CORNER_COUNT];
        static_cast<OBBBoxCollider*>(c)->GetBoundingBox().GetCorners(corners);
        minX = maxX = corners[0].x; minZ = maxZ = corners[0].z;
        for (const XMFLOAT3& p : corners)
        {
            minX = min(minX, p.x); maxX = max(maxX, p.x);
            minZ = min(minZ, p.z); maxZ = max(maxZ, p.z);
        }
        return true;
    }
    default:
        return false;
    }
}

bool LooseQuadTree::Locate(const Item& it, int& level, int& cell) const
{
    if (!m_hasBounds) return false;
    const float d = max(it.maxX - it.minX, it.maxZ - it.minZ);
    if (d > m_size) return false;   // 맵보다 큼 → outside

    // 지름 <= 칸 크기 인 가장 깊은 레벨 : 2^L <= size / d
    int L = (d <= 1e-6f) ? m_maxDepth : static_cast<int>(floorf(log2f(m_size / d)));
    L = max(0, min(L, m_maxDepth));
    while (L > 0 && d > m_cellSize[L]) --L;   // 부동소수 반올림 보정

    const float cs = m_cellSize[L];
    const float cx = 0.5f * (it.minX + it.maxX), cz = 0.5f * (it.minZ + it.maxZ);
    const int ix = static_cast<int>(floorf((cx - m_minX) / cs));
    const int iz = static_cast<int>(floorf((cz - m_minZ) / cs));
    if (ix < 0 || iz < 0 || ix >= m_dim[L] || iz >= m_dim[L]) return false;   // 중심이 맵 밖 → outside

    level = L;
    cell = iz * m_dim[L] + ix;
    return true;
}

void LooseQuadTree::Place(int idx)
{
    Item& it = m_items[idx];
    int L, cell;
    if (Locate(it, L, cell))
    {
        auto& v = m_cells[L][cell];
        it.level = L; it.cell = cell; it.slot = static_cast<int>(v.size());
        v.push_back(idx);
        ++m_levelCount[L];
    }
    else
    {
        it.level = -1; it.cell = -1; it.slot = static_cast<int>(m_outside.size());
        m_outside.push_back(idx);
    }
}

void LooseQuadTree::Unplace(int idx)
{
    Item& it = m_items[idx];
    vector<int>& v = (it.level >= 0) ? m_cells[it.level][it.cell] : m_outside;
    if (it.slot < 0 || it.slot >= static_cast<int>(v.size())) return;
    const int last = v.back();
    v[it.slot] = last;
    m_items[last].slot = it.slot;
    v.pop_back();
    if (it.level >= 0) --m_levelCount[it.level];
    it.level = -1; it.cell = -1; it.slot = -1;
}

void LooseQuadTree::FitBounds(const vector<int>& indices)
{
    float x0 = FLT_MAX, z0 = FLT_MAX, x1 = -FLT_MAX, z1 = -FLT_MAX;
    for (int i : indices)
    {
        const Item& it = m_items[i];
        x0 = min(x0, it.minX); z0 = min(z0, it.minZ);
        x1 = max(x1, it.maxX); z1 = max(z1, it.maxZ);
    }
    if (x0 > x1) { x0 = z0 = -MIN_WORLD_SIZE * 0.5f; x1 = z1 = MIN_WORLD_SIZE * 0.5f; }
    // 지금 범위의 1.5배 + 여유 32m, 정사각형
    const float size = max(MIN_WORLD_SIZE, max(x1 - x0, z1 - z0) * 1.5f + 32.f);
    const float cx = 0.5f * (x0 + x1), cz = 0.5f * (z0 + z1);
    ResetGrid(cx - size * 0.5f, cz - size * 0.5f, size);
}

void LooseQuadTree::Sync(const vector<shared_ptr<BaseCollider>>& colliders)
{
    const auto t0 = Clock::now();
    ++m_frame;
    m_stats.inserted = m_stats.removed = m_stats.moved = 0;

    vector<int> fresh;   // 처음 들어온 아이템 (월드 범위가 아직 없을 때 모아서 한 번에 넣는다)
    for (const shared_ptr<BaseCollider>& c : colliders)
    {
        if (!c) continue;
        BaseCollider* raw = c.get();
        auto found = m_lookup.find(raw);
        if (found == m_lookup.end())
        {
            int idx;
            if (!m_free.empty()) { idx = m_free.back(); m_free.pop_back(); }
            else { idx = static_cast<int>(m_items.size()); m_items.emplace_back(); }
            Item& it = m_items[idx];
            it = Item{};
            it.col = c; it.alive = true; it.seen = m_frame; it.id = raw->GetID();
            if (!ComputeXZBounds(raw, it.minX, it.minZ, it.maxX, it.maxZ))
                it.minX = it.maxX = it.minZ = it.maxZ = 0.f;
            m_lookup.emplace(raw, idx);
            if (m_hasBounds) Place(idx); else fresh.push_back(idx);
            ++m_stats.inserted;
            continue;
        }

        Item& it = m_items[found->second];
        it.seen = m_frame;
        it.id = raw->GetID();
        ComputeXZBounds(raw, it.minX, it.minZ, it.maxX, it.maxZ);

        int L = -1, cell = -1;
        const bool inside = Locate(it, L, cell);
        const bool same = inside ? (it.level == L && it.cell == cell) : (it.level < 0);
        if (!same)
        {
            Unplace(found->second);
            Place(found->second);
            ++m_stats.moved;
        }
    }

    if (!m_hasBounds)
    {
        FitBounds(fresh);
        for (int idx : fresh) Place(idx);
    }

    // 이번 프레임 목록에 없는 아이템 = 삭제 · 비활성 → 뺀다 (shared_ptr 도 여기서 놓는다)
    for (int i = 0; i < static_cast<int>(m_items.size()); ++i)
    {
        Item& it = m_items[i];
        if (!it.alive || it.seen == m_frame) continue;
        Unplace(i);
        m_lookup.erase(it.col.get());
        it.col.reset();
        it.alive = false;
        m_free.push_back(i);
        ++m_stats.removed;
    }

    // 맵 밖이 많아지면 월드 범위를 다시 잡는다 (드문 경우)
    const int alive = static_cast<int>(m_lookup.size());
    if (!m_outside.empty() && static_cast<int>(m_outside.size()) > max(8, alive / 10))
    {
        vector<int> all;
        all.reserve(alive);
        for (int i = 0; i < static_cast<int>(m_items.size()); ++i)
            if (m_items[i].alive) all.push_back(i);
        FitBounds(all);   // ResetGrid 가 칸 목록을 비운다
        for (int i : all) { m_items[i].level = -1; m_items[i].slot = -1; Place(i); }
        ++m_stats.refits;
    }

    m_stats.items = alive;
    m_stats.outside = static_cast<int>(m_outside.size());
    m_stats.syncUs = ElapsedUs(t0);
}

void LooseQuadTree::TestPair(int ia, int ib, vector<Hit>& out)
{
    const Item& A = m_items[ia];
    const Item& B = m_items[ib];

    ++m_stats.aabbTests;
    if (A.maxX < B.minX || B.maxX < A.minX || A.maxZ < B.minZ || B.maxZ < A.minZ)
        return;

    // 정밀 판정은 ID 가 작은 쪽 기준으로 (다른 방식과 같은 방향)
    const bool aFirst = A.id < B.id;
    shared_ptr<BaseCollider> lo = aFirst ? A.col : B.col;
    shared_ptr<BaseCollider> hi = aFirst ? B.col : A.col;

    ++m_stats.narrowTests;
    bool hit;
    if (FrameProfiler::s_active)
    {
        const auto t0 = Clock::now();
        hit = lo->Intersects(hi);
        m_stats.narrowUs += ElapsedUs(t0);   // 호출마다 프로파일러에 넣지 않고 합만 (측정 비용 최소화)
    }
    else hit = lo->Intersects(hi);
    if (hit)
    {
        Hit h;
        h.key = MakeKey(A.id, B.id);
        h.a = std::move(lo);
        h.b = std::move(hi);
        out.push_back(std::move(h));
        ++m_stats.hits;
    }
}

namespace
{
    // 2D 선분 (p0 → p1) 이 상자 [x0,x1]×[z0,z1] 와 겹치는지 (slab)
    bool SegmentHitsBox(float px, float pz, float dx, float dz, float x0, float z0, float x1, float z1)
    {
        float t0 = 0.f, t1 = 1.f;
        const float d[2] = { dx, dz }, p[2] = { px, pz }, lo[2] = { x0, z0 }, hi[2] = { x1, z1 };
        for (int k = 0; k < 2; ++k)
        {
            if (fabsf(d[k]) < 1e-8f)
            {
                if (p[k] < lo[k] || p[k] > hi[k]) return false;
                continue;
            }
            float a = (lo[k] - p[k]) / d[k], b = (hi[k] - p[k]) / d[k];
            if (a > b) std::swap(a, b);
            t0 = max(t0, a); t1 = min(t1, b);
            if (t0 > t1) return false;
        }
        return true;
    }
}

void LooseQuadTree::QueryRay(const Vec3& origin, const Vec3& dir, float maxDist, vector<shared_ptr<BaseCollider>>& out) const
{
    out.clear();
    const float px = origin.x, pz = origin.z;
    const float dx = dir.x * maxDist, dz = dir.z * maxDist;
    const float sx0 = min(px, px + dx), sx1 = max(px, px + dx);
    const float sz0 = min(pz, pz + dz), sz1 = max(pz, pz + dz);

    if (m_hasBounds)
    {
        for (int L = 0; L <= m_maxDepth; ++L)
        {
            if (m_levelCount[L] == 0) continue;
            const float cs = m_cellSize[L], half = cs * 0.5f;
            const int dim = m_dim[L];
            const int ix0 = max(0, static_cast<int>(floorf((sx0 - half - m_minX) / cs)));
            const int ix1 = min(dim - 1, static_cast<int>(floorf((sx1 + half - m_minX) / cs)));
            const int iz0 = max(0, static_cast<int>(floorf((sz0 - half - m_minZ) / cs)));
            const int iz1 = min(dim - 1, static_cast<int>(floorf((sz1 + half - m_minZ) / cs)));
            for (int iz = iz0; iz <= iz1; ++iz)
            {
                for (int ix = ix0; ix <= ix1; ++ix)
                {
                    const vector<int>& v = m_cells[L][iz * dim + ix];
                    if (v.empty()) continue;
                    // 칸의 느슨한 경계 (칸 + 사방 절반) 와 먼저 비교
                    const float cx0 = m_minX + ix * cs - half, cz0 = m_minZ + iz * cs - half;
                    if (!SegmentHitsBox(px, pz, dx, dz, cx0, cz0, cx0 + cs * 2.f, cz0 + cs * 2.f)) continue;
                    for (int i : v)
                    {
                        const Item& it = m_items[i];
                        if (SegmentHitsBox(px, pz, dx, dz, it.minX, it.minZ, it.maxX, it.maxZ)) out.push_back(it.col);
                    }
                }
            }
        }
    }
    for (int i : m_outside)
    {
        const Item& it = m_items[i];
        if (SegmentHitsBox(px, pz, dx, dz, it.minX, it.minZ, it.maxX, it.maxZ)) out.push_back(it.col);
    }
}

void LooseQuadTree::FindPairs(vector<Hit>& out)
{
    const auto t0 = Clock::now();
    out.clear();
    m_stats.aabbTests = m_stats.narrowTests = m_stats.hits = 0;
    m_stats.narrowUs = 0.0;

    const int count = static_cast<int>(m_items.size());
    for (int ia = 0; ia < count; ++ia)
    {
        const Item& A = m_items[ia];
        if (!A.alive || A.level < 0) continue;

        // 자기 레벨부터 위(칸이 큰) 레벨까지. 아래 레벨은 거기 객체가 올라와서 나를 찾는다
        for (int L = A.level; L >= 0; --L)
        {
            if (m_levelCount[L] == 0) continue;
            const float cs = m_cellSize[L];
            const float half = cs * 0.5f;
            const int dim = m_dim[L];
            const int ix0 = max(0, static_cast<int>(floorf((A.minX - half - m_minX) / cs)));
            const int ix1 = min(dim - 1, static_cast<int>(floorf((A.maxX + half - m_minX) / cs)));
            const int iz0 = max(0, static_cast<int>(floorf((A.minZ - half - m_minZ) / cs)));
            const int iz1 = min(dim - 1, static_cast<int>(floorf((A.maxZ + half - m_minZ) / cs)));
            const bool sameLevel = (L == A.level);

            for (int iz = iz0; iz <= iz1; ++iz)
            {
                for (int ix = ix0; ix <= ix1; ++ix)
                {
                    const vector<int>& v = m_cells[L][iz * dim + ix];
                    for (int ib : v)
                    {
                        if (sameLevel && ib <= ia) continue;   // 같은 레벨 : 인덱스 큰 상대만
                        TestPair(ia, ib, out);
                    }
                }
            }
        }
    }

    // outside 아이템 : 모든 아이템과 검사 (outside 끼리는 인덱스 큰 상대만)
    for (int io : m_outside)
    {
        for (int ib = 0; ib < count; ++ib)
        {
            const Item& B = m_items[ib];
            if (!B.alive || ib == io) continue;
            if (B.level < 0 && ib < io) continue;
            TestPair(io, ib, out);
        }
    }

    m_stats.queryUs = ElapsedUs(t0);
}
