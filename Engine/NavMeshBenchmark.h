#pragma once

// =============================================================================
// NavMesh 경로 방식 비교 벤치마크 (LOS / Corridor / Funnel)
//  - 같은 테스트셋(시작/도착 쌍)으로 세 방식을 차례로 측정
//  - Agent 를 렌더 없이 고정 dt 로 끝까지 이동시켜 도착 · 끼임 · 벽과 거리 기록
//  - 콘솔 요약 표 + CSV (..\tmp\benchmark\)
// 테스트셋 파일 한 줄:  <set> sx sy sz ex ey ez   (set = random | corner)
// =============================================================================
class NavMesh;

class NavMeshBenchmark
{
public:
	struct Config
	{
		uint32 seed = 20261110;
		int randomPairCount = 1000;
		float minPairDistance = 10.f;
		int repeat = 5;                // FindPath 반복 측정 (중앙값) - 1000쌍이라 5회로
		float agentSpeed = 3.5f;
		float simDt = 1.f / 60.f;
		float simMaxTime = 30.f;
		float stuckWindow = 0.5f;      // 이 시간 동안
		float stuckMinMove = 0.05f;    // 이만큼도 못 움직이면 끼임 1회
		float wallDistCap = 10.f;
		wstring outDir = L"..\\tmp\\benchmark\\";
		wstring testSetPath = L"..\\tmp\\benchmark\\navmesh_testset.txt";
	};

	static void Run(shared_ptr<NavMesh> _navMesh, const Config& _config);
	static bool AppendCornerPair(const wstring& _testSetPath, const Vec3& _start, const Vec3& _end);
};
