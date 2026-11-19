#include "pch.h"
#include "NavMeshAgent.h"
#include "GameObject.h"
#include "Transform.h"
#include "ModelAnimator.h"
#include "NavMesh.h"
#include "AnimationStateMachine.h"


NavMeshAgent::NavMeshAgent() : Super(ComponentType::NavMeshAgent)
{
}

NavMeshAgent::~NavMeshAgent()
{
}

void NavMeshAgent::Start()
{
    Super::Start();

    auto gameObject = GetGameObject();
    if (gameObject)
    {
        m_animator = gameObject->GetModelAnimator();

        // 씬에서 NavMesh 찾기
        auto scene = CURSCENE;
        for (auto& obj : scene->GetObjects())
        {
            auto navMesh = obj->GetFixedComponent<NavMesh>(ComponentType::NavMesh);
            if (navMesh)
            {
                m_navMesh = navMesh;
                break;
            }
        }
    }
}

void NavMeshAgent::Update()
{
    Super::Update();

    if (m_state == NavMeshAgentState::Moving)
    {
        UpdateMovement(DT);
        UpdateAnimation();
    }
}

void NavMeshAgent::SetDestination(const Vec3& destination)
{
    if (!m_navMesh) return;
    //cout << "SetNavMeshAgent\n";
    auto transform = GetTransform();
    if (!transform) return;

    Vec3 startPos = transform->GetPosition();
    Vec3 targetPos = m_navMesh->GetNearestPointOnNavMesh(destination);
    m_hasLastRequest = true;
    m_lastRequestStart = startPos;
    m_lastRequestDestination = destination;

    m_navMesh->FindPath(startPos, targetPos, m_path);

    if (m_pathDebugEnabled)
        CapturePathDebug();
    m_destination = targetPos;
    m_currentPathIndex = 0;
    m_hasLookAheadTarget = false;
    m_trail.clear();

    if (!m_path.empty())
    {
        m_state = NavMeshAgentState::Moving;
    }
}

void NavMeshAgent::Stop()
{
    m_state = NavMeshAgentState::Idle;
    m_path.clear();
    m_currentPathIndex = 0;
    //UpdateAnimation();
}

void NavMeshAgent::UpdateMovement(float dt)
{
    if (m_path.empty() || m_currentPathIndex >= m_path.size())
    {
        m_state = NavMeshAgentState::Arrived;
        return;
    }

    if (m_lookAhead > 0.f && m_path.size() >= 2 && m_navMesh)
    {
        UpdateMovementLookAhead(dt);
        return;
    }

    auto transform = GetTransform();
    if (!transform) return;

    Vec3 currentPos = transform->GetPosition();
    Vec3 targetPos = m_path[m_currentPathIndex];

    // 도착 판정은 수평(XZ) 거리로 한다. 이동이 수평으로만 이뤄지므로,
    // 3D 거리로 재면 웨이포인트와 지면의 높이 차 때문에 영영 '도착'하지 못하고 제자리에서 끼일 수 있다.
    const float toTargetX = targetPos.x - currentPos.x;
    const float toTargetZ = targetPos.z - currentPos.z;
    float distance = sqrtf(toTargetX * toTargetX + toTargetZ * toTargetZ);

    // 현재 웨이포인트에 도달했으면 다음으로
    if (distance <= m_stoppingDistance)
    {
        m_currentPathIndex++;
        if (m_currentPathIndex >= m_path.size())
        {
            m_state = NavMeshAgentState::Arrived;
            return;
        }
        targetPos = m_path[m_currentPathIndex];
    }

    // 이동 처리
    MoveToward(currentPos, targetPos, dt);
}

// 목표점 방향으로 한 프레임 이동 (회전 · NavMesh 위치 보정 포함). 기존 이동 처리를 그대로 옮긴 것
void NavMeshAgent::MoveToward(const Vec3& currentPos, const Vec3& targetPos, float dt)
{
    auto transform = GetTransform();
    if (!transform) return;

    Vec3 direction = targetPos - currentPos;
    direction.y = 0; // Y축 고정

    const float distXZ = direction.Length();
    if (distXZ > 0.01f)
    {
        direction.Normalize();

        // 회전 계산 및 적용
        float targetYaw = atan2(direction.x, direction.z) + 3.141592f; //3.141592 더해야 방향 제대로 됨

        //cout << "TargetYaw : " << targetYaw * 57.2958f << "\n";
        Vec3 currentRotation = transform->GetLocalRotation();
        Vec3 newRotation = Vec3(currentRotation.x, targetYaw * 180.0f / 3.14159f, currentRotation.z);
        transform->SetLocalRotation(newRotation);

        // 위치 업데이트
        // 한 프레임 이동은 목표까지 남은 거리까지만. 고정 거리(0.146)로 움직이면 짧은 선분의 웨이포인트를
        // 지나쳤다가 다음 프레임에 되돌아와 180도 회전이 생긴다 (Funnel 의 같은 꼭짓점 코너 사이 0.02~0.03 선분)
        const float step = min(m_speed * dt * 2.5f, distXZ);
        Vec3 newPos = currentPos + direction * step;
        if (m_navMesh)
        {
            Vec3 snapped = m_navMesh->CorrectAgentPosition(newPos);   // F6: Edge(최근접점) / Vertex(기존)
            if (m_pathDebugEnabled)
                RecordSnap(newPos, snapped);
            newPos = snapped;
        }
        transform->SetPosition(newPos);
        if (m_pathDebugEnabled && m_trail.size() < 4096)
            m_trail.push_back(newPos);
    }
}

// =============================================================================
// Look-ahead 추종 (Pure Pursuit)
//  경로 = Corridor 결과 그대로 (중점 기반, 통로 판정으로 안전).
//  매 프레임: ① 지금 따라가는 구간을 갱신  ② 그 구간 위 내 투영점에서 경로를 따라 L 만큼 앞의 점 = 목표점
//            ③ 내 위치 → 목표점 선분이 NavMesh 위인지 판정 (실패하면 L 을 절반씩, 3번)  ④ 목표점 쪽으로 이동
//  → 웨이포인트에 닿기 전부터 목표점이 다음 구간으로 넘어가므로 코너 앞에서 미리 둥글게 꺾는다.
// =============================================================================
namespace
{
    float SegmentProjectXZ(const Vec3& p, const Vec3& a, const Vec3& b, float& outT)
    {
        const float abx = b.x - a.x, abz = b.z - a.z;
        const float len2 = abx * abx + abz * abz;
        outT = (len2 > 1e-12f) ? ((p.x - a.x) * abx + (p.z - a.z) * abz) / len2 : 1.f;
        const float t = min(1.f, max(0.f, outT));
        const float qx = a.x + abx * t - p.x, qz = a.z + abz * t - p.z;
        return sqrtf(qx * qx + qz * qz);
    }

    float LenXZ(const Vec3& a, const Vec3& b)
    {
        const float dx = a.x - b.x, dz = a.z - b.z;
        return sqrtf(dx * dx + dz * dz);
    }
}

Vec3 NavMeshAgent::ComputeLookAheadTarget(const Vec3& pos, float lookAhead) const
{
    // 현재 구간 (idx-1 → idx) 위 투영점에서 출발해 경로를 따라 lookAhead 만큼 전진
    const size_t idx = m_currentPathIndex;
    float t;
    SegmentProjectXZ(pos, m_path[idx - 1], m_path[idx], t);
    t = min(1.f, max(0.f, t));
    Vec3 cur = m_path[idx - 1] + (m_path[idx] - m_path[idx - 1]) * t;

    float remain = lookAhead;
    for (size_t k = idx; k < m_path.size(); ++k)
    {
        const float seg = LenXZ(cur, m_path[k]);
        if (seg >= remain)
            return cur + (m_path[k] - cur) * (seg > 1e-6f ? remain / seg : 0.f);
        remain -= seg;
        cur = m_path[k];
    }
    return m_path.back();   // 경로 끝을 넘으면 도착점
}

void NavMeshAgent::UpdateMovementLookAhead(float dt)
{
    auto transform = GetTransform();
    if (!transform) return;

    const Vec3 pos = transform->GetPosition();
    const size_t last = m_path.size() - 1;
    if (m_currentPathIndex == 0)
        m_currentPathIndex = 1;

    // ① 따라가는 구간 갱신: 구간 끝을 지났거나 / 다음 구간이 더 가깝거나 / 웨이포인트에 닿았으면 다음 구간으로
    while (m_currentPathIndex < last)
    {
        const size_t i = m_currentPathIndex;
        float tCur, tNext;
        const float dCur = SegmentProjectXZ(pos, m_path[i - 1], m_path[i], tCur);
        const float dNext = SegmentProjectXZ(pos, m_path[i], m_path[i + 1], tNext);
        if (tCur >= 1.f || dNext <= dCur || LenXZ(pos, m_path[i]) <= m_stoppingDistance)
            ++m_currentPathIndex;
        else
            break;
    }

    // 도착
    if (m_currentPathIndex == last && LenXZ(pos, m_path[last]) <= m_stoppingDistance)
    {
        m_currentPathIndex = static_cast<uint32>(m_path.size());
        m_state = NavMeshAgentState::Arrived;
        m_hasLookAheadTarget = false;
        return;
    }

    // ② ③ 목표점 계산 + 판정 (실패하면 L 절반, 3번 다 실패하면 기존처럼 현재 웨이포인트)
    Vec3 target = m_path[m_currentPathIndex];
    float lookAhead = m_lookAhead;
    for (int attempt = 0; attempt < 3; ++attempt, lookAhead *= 0.5f)
    {
        const Vec3 candidate = ComputeLookAheadTarget(pos, lookAhead);
        if (m_navMesh->IsSegmentOnNavMesh(pos, candidate))
        {
            target = candidate;
            break;
        }
    }
    m_lookAheadTarget = target;
    m_hasLookAheadTarget = true;

    // ④ 이동. 목표점을 지나치지 않도록 한 프레임 이동량은 목표점까지 거리로 제한
    const float step = m_speed * dt * 2.5f;
    const float toTarget = LenXZ(pos, target);
    MoveToward(pos, target, (step > 1e-6f && toTarget < step) ? dt * (toTarget / step) : dt);
}

void NavMeshAgent::UpdateAnimation()
{
    if (!m_animator) return;

    auto gameObject = GetGameObject();
    if (gameObject)
    {
        auto stateMachine = gameObject->GetFixedComponent<AnimationStateMachine>(ComponentType::AnimationStateMachine);
        if (stateMachine)
        {
            switch (m_state)
            {
            case NavMeshAgentState::Moving:
                if (!stateMachine->IsInState(AnimationStateType::Run))
                {
                    //stateMachine->ChangeState(AnimationStateType::Run);
                }
                break;
            case NavMeshAgentState::Idle:
            case NavMeshAgentState::Arrived:
                if (!stateMachine->IsInState(AnimationStateType::Wait))
                {
                   // stateMachine->ChangeState(AnimationStateType::Wait);
                }
                break;
            }
        }
    }
}

// =============================================================================
// 경로 진단
// =============================================================================
namespace
{
    constexpr float OFFMESH_SAMPLE_STEP = 0.1f;   // 최종 경로 검사 간격
    constexpr float SNAP_RECORD_DIST = 0.05f;     // 이 이상 보정되면 기록
    constexpr float SNAP_LOG_DIST = 0.3f;         // 이 이상 보정되면 로그
    constexpr size_t SNAP_HISTORY = 64;

    float DistanceXZ(const Vec3& a, const Vec3& b)
    {
        float dx = a.x - b.x, dz = a.z - b.z;
        return sqrtf(dx * dx + dz * dz);
    }
}

void NavMeshAgent::CapturePathDebug()
{
    m_lastPathDebug = m_navMesh->GetLastPathDebug();
    m_offMeshSamples.clear();

    // 최종 경로를 촘촘히 샘플링해서 어떤 삼각형에도 속하지 않는 지점 찾기
    vector<int> badSegments;
    const vector<Vec3>& path = m_lastPathDebug.finalPath;
    for (size_t i = 1; i < path.size(); ++i)
    {
        const Vec3& a = path[i - 1];
        const Vec3& b = path[i];
        float length = DistanceXZ(a, b);
        int steps = max(1, static_cast<int>(length / OFFMESH_SAMPLE_STEP));
        bool bad = false;
        for (int s = 0; s <= steps; ++s)
        {
            Vec3 p = a + (b - a) * (static_cast<float>(s) / steps);
            // Grid 판정이 실패해도 경계 오차 허용 전수 검사로 다시 확인 (꼭짓점/변 위의 점 오판 방지)
            if (!m_navMesh->IsPointInsideNavMesh(p) && m_navMesh->FindTriangleBruteForce(p, 1e-4f) == -1)
            {
                m_offMeshSamples.push_back(p);
                bad = true;
            }
        }
        if (bad)
            badSegments.push_back(static_cast<int>(i - 1));
    }

    const NavPathDebugInfo& d = m_lastPathDebug;
    cout << "[PathDebug] " << NavMesh::BranchName(d.branch)
         << (d.branch == NavPathBranch::AStarSmoothed ? string(" [") + NavMesh::SmoothingModeName(d.smoothingMode) + "]" : string(""))
         << " r=" << m_navMesh->GetAgentRadius() << (m_navMesh->IsEdgeSnap() ? " snap=Edge" : " snap=Vertex")
         << " | startTri " << d.startTriangle << " endTri " << d.endTriangle
         << " | raw " << d.rawPath.size() << " -> final " << d.finalPath.size()
         << " | offMesh " << m_offMeshSamples.size();
    if (!badSegments.empty())
    {
        cout << " (segments";
        for (int s : badSegments) cout << ' ' << s;
        cout << ")";
    }
    cout << endl;

    if (d.branch == NavPathBranch::TriangleNotFound)
    {
        if (d.startTriangle == -1)
            cout << "  - start (" << d.start.x << ", " << d.start.y << ", " << d.start.z << ") 삼각형 없음 → 전수 검사 "
                 << d.bruteStartTriangle << (d.bruteStartTriangle == -1 ? " (실제로 NavMesh 밖)" : " (Grid/경계 판정 문제)") << endl;
        if (d.endTriangle == -1)
            cout << "  - end (" << d.end.x << ", " << d.end.y << ", " << d.end.z << ") 삼각형 없음 → 전수 검사 "
                 << d.bruteEndTriangle << (d.bruteEndTriangle == -1 ? " (실제로 NavMesh 밖)" : " (Grid/경계 판정 문제)") << endl;
    }
}

void NavMeshAgent::RecordSnap(const Vec3& before, const Vec3& after)
{
    float dist = DistanceXZ(before, after);
    if (dist < SNAP_RECORD_DIST)
        return;

    ++m_snapCount;
    if (m_snapPoints.size() < SNAP_HISTORY)
        m_snapPoints.push_back(after);
    else
        m_snapPoints[m_snapWriteIndex] = after;
    m_snapWriteIndex = (m_snapWriteIndex + 1) % SNAP_HISTORY;

    if (dist >= SNAP_LOG_DIST)
    {
        cout << "[PathDebug] 이동 보정 " << dist << " : (" << before.x << ", " << before.z << ") -> ("
             << after.x << ", " << after.z << ") waypoint " << m_currentPathIndex << "/" << m_path.size() << endl;
    }
}

void NavMeshAgent::DrawPathDebug(const Matrix& view, const Matrix& projection)
{
    if (!m_pathDebugEnabled)
        return;

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    Viewport viewport = GRAPHICS->GetViewport();

    auto toScreen = [&](const Vec3& p, ImVec2& out) -> bool {
        Vec3 s = viewport.Project(p, Matrix::Identity, view, projection);
        if (s.z < 0.f || s.z > 1.f)
            return false;
        out = ImVec2(s.x, s.y);
        return true;
    };

    auto drawPolyline = [&](const vector<Vec3>& pts, ImU32 color, float thickness) {
        for (size_t i = 1; i < pts.size(); ++i)
        {
            ImVec2 a, b;
            if (toScreen(pts[i - 1], a) && toScreen(pts[i], b))
                drawList->AddLine(a, b, color, thickness);
        }
        for (const Vec3& p : pts)
        {
            ImVec2 s;
            if (toScreen(p, s))
                drawList->AddCircleFilled(s, thickness + 1.5f, color);
        }
    };

    const ImU32 YELLOW = IM_COL32(255, 220, 0, 255);
    const ImU32 CYAN = IM_COL32(0, 220, 255, 255);
    const ImU32 MAGENTA = IM_COL32(255, 0, 255, 255);
    const ImU32 ORANGE = IM_COL32(255, 140, 0, 255);
    const ImU32 WHITE = IM_COL32(255, 255, 255, 255);

    drawPolyline(m_lastPathDebug.rawPath, YELLOW, 1.5f);

    // Funnel 입력 Portal: 얇은 선 + left(초록) / right(보라)
    const ImU32 PORTAL = IM_COL32(255, 190, 90, 150);
    const ImU32 PORTAL_L = IM_COL32(46, 200, 110, 255);
    const ImU32 PORTAL_R = IM_COL32(150, 110, 230, 255);
    for (const auto& portal : m_lastPathDebug.portals)
    {
        ImVec2 l, r;
        bool hasL = toScreen(portal.first, l);
        bool hasR = toScreen(portal.second, r);
        if (hasL && hasR)
            drawList->AddLine(l, r, PORTAL, 1.0f);
        if (hasL)
            drawList->AddCircleFilled(l, 2.5f, PORTAL_L);
        if (hasR)
            drawList->AddCircleFilled(r, 2.5f, PORTAL_R);
    }
    drawPolyline(m_lastPathDebug.finalPath, CYAN, 3.0f);

    for (const Vec3& p : m_offMeshSamples)
    {
        ImVec2 s;
        if (toScreen(p, s))
            drawList->AddCircleFilled(s, 3.0f, MAGENTA);
    }
    for (const Vec3& p : m_snapPoints)
    {
        ImVec2 s;
        if (toScreen(p, s))
            drawList->AddCircle(s, 5.0f, ORANGE, 0, 2.0f);
    }

    // 현재 향하는 웨이포인트
    if (m_state == NavMeshAgentState::Moving && m_currentPathIndex < m_path.size())
    {
        ImVec2 s;
        if (toScreen(m_path[m_currentPathIndex], s))
            drawList->AddCircle(s, 8.0f, WHITE, 0, 2.0f);
    }

    // Look-ahead: 실제 궤적(연두) + 지금 향하는 목표점(분홍)
    const ImU32 TRAIL = IM_COL32(150, 255, 90, 255);
    const ImU32 PINK = IM_COL32(255, 110, 180, 255);
    for (size_t i = 1; i < m_trail.size(); ++i)
    {
        ImVec2 a, b;
        if (toScreen(m_trail[i - 1], a) && toScreen(m_trail[i], b))
            drawList->AddLine(a, b, TRAIL, 2.0f);
    }
    if (m_state == NavMeshAgentState::Moving && m_hasLookAheadTarget)
    {
        ImVec2 s;
        if (toScreen(m_lookAheadTarget, s))
            drawList->AddCircleFilled(s, 5.0f, PINK);
    }

    // 좌상단 요약
    char buf[384];
    sprintf_s(buf, "[PathDebug F8] smooth(F7) %s | lookAhead(F4) %.1f | radius(F5) %.2f | snap(F6) %s | %s | startTri %d endTri %d | raw %zu -> final %zu | offMesh %zu | snaps %d",
        m_navMesh ? NavMesh::SmoothingModeName(m_navMesh->GetSmoothingMode()) : "-",
        m_lookAhead,
        m_navMesh ? m_navMesh->GetAgentRadius() : 0.f,
        (m_navMesh && m_navMesh->IsEdgeSnap()) ? "Edge" : "Vertex",
        NavMesh::BranchName(m_lastPathDebug.branch), m_lastPathDebug.startTriangle, m_lastPathDebug.endTriangle,
        m_lastPathDebug.rawPath.size(), m_lastPathDebug.finalPath.size(), m_offMeshSamples.size(), m_snapCount);
    drawList->AddRectFilled(ImVec2(8, 8), ImVec2(8 + ImGui::CalcTextSize(buf).x + 12, 30), IM_COL32(0, 0, 0, 170), 4.0f);
    drawList->AddText(ImVec2(14, 11), WHITE, buf);
    drawList->AddText(ImVec2(14, 34), WHITE, "yellow: A* raw   cyan: final   magenta: off-mesh   orange: snap   white: target wp   portal L green / R purple   lime: trail   pink: look-ahead target");
}

// 벤치마크용: 애니메이션 없이 고정 dt 로 이동만
void NavMeshAgent::Simulate(float dt)
{
    if (m_state == NavMeshAgentState::Moving)
        UpdateMovement(dt);
}

bool NavMeshAgent::GetLastRequest(Vec3& outStart, Vec3& outDestination) const
{
    if (!m_hasLastRequest)
        return false;
    outStart = m_lastRequestStart;
    outDestination = m_lastRequestDestination;
    return true;
}
