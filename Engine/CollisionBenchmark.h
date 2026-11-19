#pragma once

// =============================================================================
// CollisionBenchmark : 충돌 broadphase 비교 기록기
//  - 기록 중인 프레임마다 Scene 이 같은 충돌체 상태로 세 가지를 잰다
//      BruteForce (정답) · ScreenQuadTree (기존 화면 좌표 트리) · LooseQuadTree (월드 XZ)
//  - 기록이 끝나면 CSV (..\tmp\benchmark\collision_*.csv) 와 콘솔 요약을 남긴다
// =============================================================================
struct CollisionBenchRow
{
    int frame = 0;
    int eligible = 0;               // 활성 객체 + 활성 충돌체 수

    // 기존 : 화면 좌표 QuadTree
    int screenInserted = 0;         // 트리에 들어간 객체 수 (화면 안)
    double screenUpdateUs = 0.0;    // UpdateQuadTree 전체 (변화 감지 + 재생성). FOW · Picking 도 이 트리를 쓴다
    int screenRebuilt = 0;          // 이번 프레임 재생성 여부
    int screenNodesCreated = 0;     // 이번 프레임 새로 만든 노드 수
    double screenQueryUs = 0.0;     // 쌍 찾기 + 정밀 판정
    long long screenNarrow = 0;
    int screenPairs = 0;
    int screenMissed = 0;           // 정답에는 있는데 없는 쌍
    int screenExtra = 0;            // 정답에는 없는데 있는 쌍

    // 새 방식 : 월드 XZ Loose QuadTree
    double looseSyncUs = 0.0;       // 넣기 · 옮기기 · 빼기
    double looseQueryUs = 0.0;      // 쌍 찾기 + 정밀 판정
    int looseInserted = 0, looseRemoved = 0, looseMoved = 0, looseOutside = 0;
    long long looseAabb = 0;
    long long looseNarrow = 0;
    int loosePairs = 0;
    int looseMissed = 0;
    int looseExtra = 0;

    // 정답 : 전부 짝짓기
    double bruteUs = 0.0;
    long long bruteNarrow = 0;
    int brutePairs = 0;

    // 이벤트를 실제로 낸 방식 (0 = Screen, 1 = Loose) 과, 그 방식의 '닿음' 상태 중 정답에 없는 것 (Exit 누락)
    int driver = 0;
    int driverStale = 0;
};

class CollisionBenchmark
{
public:
    void Start(int frames, int driver);
    void Cancel();
    bool IsRecording() const { return m_recording; }
    int GetRecorded() const { return static_cast<int>(m_rows.size()); }
    int GetTarget() const { return m_target; }

    // 목표 프레임 수가 차면 자동으로 CSV + 요약을 남기고 끝난다
    void AddRow(const CollisionBenchRow& row);

    wstring outDir = L"..\\tmp\\benchmark\\";

private:
    void Finish();

    vector<CollisionBenchRow> m_rows;
    int m_target = 0;
    bool m_recording = false;
};
