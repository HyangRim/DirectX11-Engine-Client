#include "pch.h"
#include "NavMeshBenchmark.h"
#include "NavMesh.h"
#include "NavMeshAgent.h"
#include "GameObject.h"
#include "Transform.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <queue>
#include <random>
#include <sstream>

namespace
{
	struct Pair { string set; Vec3 s, e; };

	struct Result
	{
		bool arrived = false;
		int stuck = 0;
		float offMeshDepth = 0.f;   // 최종 경로가 NavMesh 밖으로 나간 최대 깊이 (0 = 침범 없음)
		float pathLen = 0.f;
		float lenRatio = 0.f;       // 최단 기준(Funnel, 반경 0) 대비
		float minWall = 0.f;
		double findPathUs = 0.0;    // FindPath 전체
		double astarUs = 0.0;       // A* + 원시 경로 변환 (세 방식 공통)
		double smoothUs = 0.0;      // 스무딩만 (방식 간 순수 비교값)
		float simTime = 0.f;
		int waypoints = 0;
		float moveLen = 0.f;        // 실제로 이동한 거리 (XZ)
		float maxTurnDeg = 0.f;     // 한 프레임에 꺾인 최대 각도 (클수록 각지게 꺾임)
		int sharpTurns = 0;         // 한 프레임에 30도 이상 꺾인 횟수
	};

	struct ModeSpec { NavSmoothingMode mode; float lookAhead; const char* name; };

	double NowUs()
	{
		static LARGE_INTEGER f = []() { LARGE_INTEGER x; ::QueryPerformanceFrequency(&x); return x; }();
		LARGE_INTEGER c; ::QueryPerformanceCounter(&c);
		return static_cast<double>(c.QuadPart) * 1'000'000.0 / static_cast<double>(f.QuadPart);
	}

	template<typename T> T Median(vector<T> v)
	{
		if (v.empty()) return T();
		std::sort(v.begin(), v.end());
		return v[v.size() / 2];
	}

	float DistXZ(const Vec3& a, const Vec3& b) { const float dx = a.x - b.x, dz = a.z - b.z; return sqrtf(dx * dx + dz * dz); }

	float PathLength(const vector<Vec3>& p)
	{
		float len = 0.f;
		for (size_t i = 1; i < p.size(); ++i) len += DistXZ(p[i - 1], p[i]);
		return len;
	}

	// ---------------------------------------------------------------- 경계 변(벽) 거리
	class Walls
	{
	public:
		void Build(const NavMesh& nav)
		{
			for (int t = 0; t < nav.GetTriangleCount(); ++t)
			{
				const NavMeshTriangle& tri = nav.GetTriangle(t);
				for (int e = 0; e < 3; ++e)
				{
					if (tri.edgeNeighbor[e] != -1) continue;
					const Vec3& a = tri.vertices[e];
					const Vec3& b = tri.vertices[(e + 1) % 3];
					const int idx = static_cast<int>(m_edges.size());
					m_edges.emplace_back(a, b);
					for (int x = Cell(min(a.x, b.x)); x <= Cell(max(a.x, b.x)); ++x)
						for (int z = Cell(min(a.z, b.z)); z <= Cell(max(a.z, b.z)); ++z)
							m_grid[Key(x, z)].push_back(idx);
				}
			}
		}

		float Distance(const Vec3& p, float cap) const
		{
			const int r = static_cast<int>(ceilf(cap / CELL));
			float best = cap;
			for (int x = Cell(p.x) - r; x <= Cell(p.x) + r; ++x)
				for (int z = Cell(p.z) - r; z <= Cell(p.z) + r; ++z)
				{
					auto it = m_grid.find(Key(x, z));
					if (it == m_grid.end()) continue;
					for (int i : it->second) best = min(best, SegDist(p, m_edges[i].first, m_edges[i].second));
				}
			return best;
		}

		size_t Count() const { return m_edges.size(); }

	private:
		static constexpr float CELL = 5.f;
		static int Cell(float v) { return static_cast<int>(floorf(v / CELL)); }
		static uint64 Key(int x, int z) { return (static_cast<uint64>(static_cast<uint32>(x)) << 32) | static_cast<uint32>(z); }
		static float SegDist(const Vec3& p, const Vec3& a, const Vec3& b)
		{
			const float abx = b.x - a.x, abz = b.z - a.z, apx = p.x - a.x, apz = p.z - a.z;
			const float l2 = abx * abx + abz * abz;
			const float t = (l2 > 1e-8f) ? max(0.f, min(1.f, (apx * abx + apz * abz) / l2)) : 0.f;
			const float dx = apx - abx * t, dz = apz - abz * t;
			return sqrtf(dx * dx + dz * dz);
		}
		vector<pair<Vec3, Vec3>> m_edges;
		unordered_map<uint64, vector<int>> m_grid;
	};

	// ---------------------------------------------------------------- 테스트셋
	vector<Pair> LoadPairs(const wstring& path)
	{
		vector<Pair> out;
		std::ifstream f(path);
		string line;
		while (std::getline(f, line))
		{
			if (line.empty() || line[0] == '#') continue;
			std::istringstream ss(line);
			Pair p;
			if (ss >> p.set >> p.s.x >> p.s.y >> p.s.z >> p.e.x >> p.e.y >> p.e.z) out.push_back(p);
		}
		return out;
	}

	void WritePair(std::ostream& o, const string& set, const Vec3& s, const Vec3& e)
	{
		o << set << ' ' << std::setprecision(9) << s.x << ' ' << s.y << ' ' << s.z << ' ' << e.x << ' ' << e.y << ' ' << e.z << '\n';
	}

	void EnsureDir(const wstring& dir)
	{
		for (size_t pos = dir.find_first_of(L"\\/"); pos != wstring::npos; pos = dir.find_first_of(L"\\/", pos + 1))
		{
			const wstring sub = dir.substr(0, pos);
			if (!sub.empty() && sub != L"..") ::CreateDirectoryW(sub.c_str(), nullptr);
		}
	}

	vector<Pair> GenerateRandom(const NavMesh& nav, const NavMeshBenchmark::Config& cfg)
	{
		const int n = nav.GetTriangleCount();
		vector<double> area(n);
		vector<int> comp(n, -1);
		for (int i = 0; i < n; ++i)
		{
			const NavMeshTriangle& t = nav.GetTriangle(i);
			area[i] = 0.5 * (t.vertices[1] - t.vertices[0]).Cross(t.vertices[2] - t.vertices[0]).Length();
		}
		int next = 0;   // 연결 요소 (서로 이어지지 않은 섬끼리는 쌍을 만들지 않음)
		for (int i = 0; i < n; ++i)
		{
			if (comp[i] != -1) continue;
			std::queue<int> q; q.push(i); comp[i] = next;
			while (!q.empty())
			{
				const int c = q.front(); q.pop();
				for (int nb : nav.GetTriangle(c).neighbors)
					if (comp[nb] == -1) { comp[nb] = next; q.push(nb); }
			}
			++next;
		}

		std::mt19937 rng(cfg.seed);
		std::discrete_distribution<int> pick(area.begin(), area.end());
		std::uniform_real_distribution<float> u(0.f, 1.f);
		auto sample = [&](int& tri) {
			tri = pick(rng);
			const NavMeshTriangle& t = nav.GetTriangle(tri);
			const float r1 = sqrtf(u(rng)), r2 = u(rng);
			return t.vertices[0] * (1.f - r1) + t.vertices[1] * (r1 * (1.f - r2)) + t.vertices[2] * (r1 * r2);
		};

		vector<Pair> out;
		for (int a = 0; a < cfg.randomPairCount * 1000 && static_cast<int>(out.size()) < cfg.randomPairCount; ++a)
		{
			int ts, te;
			const Vec3 s = sample(ts), e = sample(te);
			if (comp[ts] != comp[te] || DistXZ(s, e) < cfg.minPairDistance) continue;
			out.push_back({ "random", s, e });
		}
		return out;
	}

	// ---------------------------------------------------------------- 쌍 하나 측정 (현재 스무딩 방식)
	Result Measure(const shared_ptr<NavMesh>& nav, const Walls& walls, const Pair& p, const NavMeshBenchmark::Config& cfg, float shortestLen, float lookAhead)
	{
		Result r;
		vector<Vec3> path;

		// 1) FindPath 시간 (중앙값)
		vector<double> times, astar, smooth;
		for (int i = 0; i < max(1, cfg.repeat); ++i)
		{
			const double t0 = NowUs();
			nav->FindPath(p.s, p.e, path);
			times.push_back(NowUs() - t0);
			astar.push_back(nav->GetLastAStarUs());
			smooth.push_back(nav->GetLastSmoothUs());
		}
		r.findPathUs = Median(times);
		r.astarUs = Median(astar);
		r.smoothUs = Median(smooth);
		r.pathLen = PathLength(path);
		r.lenRatio = (shortestLen > 1e-4f) ? r.pathLen / shortestLen : 0.f;
		r.waypoints = static_cast<int>(path.size());

		// 2) 벽 침범: 경로를 0.1 간격으로 검사, 밖이면 NavMesh 까지 거리
		for (size_t i = 1; i < path.size(); ++i)
		{
			const int steps = max(1, static_cast<int>(DistXZ(path[i - 1], path[i]) / 0.1f));
			for (int k = 0; k <= steps; ++k)
			{
				const Vec3 q = path[i - 1] + (path[i] - path[i - 1]) * (static_cast<float>(k) / steps);
				if (!nav->IsPointInsideNavMesh(q))
					r.offMeshDepth = max(r.offMeshDepth, DistXZ(q, nav->GetClosestPointOnNavMesh(q)));
			}
		}

		// 3) 시뮬레이션: 렌더 없이 고정 dt
		auto obj = make_shared<GameObject>();
		obj->GetTransform()->SetPosition(p.s);
		auto agent = make_shared<NavMeshAgent>();
		obj->AddComponent(agent);
		agent->SetNavMesh(nav);
		agent->SetSpeed(cfg.agentSpeed);
		agent->SetLookAhead(lookAhead);
		agent->SetDestination(p.e);

		float t = 0.f, window = 0.f;
		bool stuckNow = false;
		Vec3 anchor = p.s;
		Vec3 prevPos = p.s;
		float prevDirX = 0.f, prevDirZ = 0.f;
		bool hasPrevDir = false;
		r.minWall = walls.Distance(p.s, cfg.wallDistCap);
		while (agent->IsMoving() && t < cfg.simMaxTime)
		{
			agent->Simulate(cfg.simDt);
			t += cfg.simDt;
			window += cfg.simDt;
			const Vec3 pos = obj->GetTransform()->GetPosition();
			r.minWall = min(r.minWall, walls.Distance(pos, cfg.wallDistCap));

			// 이동 거리 · 프레임당 꺾인 각도
			const float mdx = pos.x - prevPos.x, mdz = pos.z - prevPos.z;
			const float mlen = sqrtf(mdx * mdx + mdz * mdz);
			r.moveLen += mlen;
			if (mlen > 1e-3f)
			{
				const float dirX = mdx / mlen, dirZ = mdz / mlen;
				if (hasPrevDir)
				{
					const float c = max(-1.f, min(1.f, dirX * prevDirX + dirZ * prevDirZ));
					const float deg = acosf(c) * 57.29578f;
					r.maxTurnDeg = max(r.maxTurnDeg, deg);
					if (deg >= 30.f) ++r.sharpTurns;
				}
				prevDirX = dirX; prevDirZ = dirZ; hasPrevDir = true;
			}
			prevPos = pos;
			if (window >= cfg.stuckWindow)
			{
				const bool noMove = DistXZ(pos, anchor) < cfg.stuckMinMove;
				if (noMove && !stuckNow) ++r.stuck;
				stuckNow = noMove;
				anchor = pos;
				window = 0.f;
			}
		}
		r.arrived = agent->HasReachedDestination();
		r.simTime = t;
		return r;
	}
}

void NavMeshBenchmark::Run(shared_ptr<NavMesh> nav, const Config& cfg)
{
	if (!nav || nav->GetTriangleCount() == 0) { printf("[Benchmark] NavMesh is empty\n"); return; }
	const double startUs = NowUs();
	EnsureDir(cfg.outDir);

	// 테스트셋: 파일이 있으면 재사용, random 개수가 설정과 다르면 다시 생성 후 저장 (corner 는 유지)
	vector<Pair> pairs = LoadPairs(cfg.testSetPath);
	const auto randomCount = std::count_if(pairs.begin(), pairs.end(), [](const Pair& p) { return p.set == "random"; });
	if (randomCount != cfg.randomPairCount)
	{
		vector<Pair> gen = GenerateRandom(*nav, cfg);
		for (const Pair& p : pairs)
			if (p.set != "random") gen.push_back(p);
		pairs = gen;
		std::ofstream f(cfg.testSetPath, std::ios::trunc);
		f << "# NavMesh benchmark test set (seed " << cfg.seed << ")\n";
		for (const Pair& p : pairs) WritePair(f, p.set, p.s, p.e);
	}

	Walls walls;
	walls.Build(*nav);

	const NavSmoothingMode savedMode = nav->GetSmoothingMode();
	const float savedRadius = nav->GetAgentRadius();

	// 최단 기준: Funnel + 반경 0 (같은 A* 통로 안의 최단 경로)
	vector<float> shortest(pairs.size(), 0.f);
	nav->SetSmoothingMode(NavSmoothingMode::Funnel);
	nav->SetAgentRadius(0.f);
	for (size_t i = 0; i < pairs.size(); ++i)
	{
		vector<Vec3> path;
		nav->FindPath(pairs[i].s, pairs[i].e, path);
		shortest[i] = PathLength(path);
	}
	nav->SetAgentRadius(savedRadius);

	// 비교 대상만 남김: LOS · CorridorFan · CorridorFanPortal 은 측정 완료로 제외 (F7 로는 계속 볼 수 있음)
	//   Corridor (통로 판정 기준) → Funnel → Funnel + LookAhead → FunnelCurve → FunnelCurveAngle → FunnelCurveTangent
	constexpr int MODE_COUNT = 6;
	const ModeSpec modes[MODE_COUNT] = {
		{ NavSmoothingMode::Corridor,           0.f, "Corridor" },
		{ NavSmoothingMode::Funnel,             0.f, "Funnel" },
		{ NavSmoothingMode::Funnel,             1.f, "FunnelLA1" },
		{ NavSmoothingMode::FunnelCurve,        0.f, "FunnelCurve" },
		{ NavSmoothingMode::FunnelCurveAngle,   0.f, "FunnelCurveAngle" },
		{ NavSmoothingMode::FunnelCurveTangent, 0.f, "FunnelCurveTangent" } };
	vector<Result> results[MODE_COUNT];
	printf("[Benchmark] %zu pairs x %d modes (repeat %d, edgeSnap %s, funnel radius %.2f) ... running\n",
		pairs.size(), MODE_COUNT, cfg.repeat, nav->IsEdgeSnap() ? "Edge" : "Vertex", savedRadius);
	for (int m = 0; m < MODE_COUNT; ++m)
	{
		nav->SetSmoothingMode(modes[m].mode);
		for (size_t i = 0; i < pairs.size(); ++i)
		{
			results[m].push_back(Measure(nav, walls, pairs[i], cfg, shortest[i], modes[m].lookAhead));
			if ((i + 1) % 100 == 0 || i + 1 == pairs.size())
				printf("[Benchmark] %-8s %zu / %zu  (%.0f s)\n", modes[m].name, i + 1, pairs.size(), (NowUs() - startUs) / 1e6);
		}
	}
	nav->SetSmoothingMode(savedMode);

	// 요약 표
	printf("\n[Benchmark] done in %.1f s\n", (NowUs() - startUs) / 1e6);
	printf("%-14s | arrive%% | stuck(pairs/total) | offMesh(pairs, max) | len ratio(avg) | minWall(min/avg) | findPath / A* / smooth us(median)\n", "mode");
	for (int m = 0; m < MODE_COUNT; ++m)
	{
		const vector<Result>& rs = results[m];
		int arrived = 0, stuckPairs = 0, stuckTotal = 0, offPairs = 0;
		float offMax = 0.f, ratioSum = 0.f, wallMin = cfg.wallDistCap, wallSum = 0.f;
		vector<double> times, astar, smooth;
		for (const Result& r : rs)
		{
			arrived += r.arrived; stuckPairs += (r.stuck > 0); stuckTotal += r.stuck;
			offPairs += (r.offMeshDepth > 0.01f); offMax = max(offMax, r.offMeshDepth);
			ratioSum += r.lenRatio; wallMin = min(wallMin, r.minWall); wallSum += r.minWall;
			times.push_back(r.findPathUs); astar.push_back(r.astarUs); smooth.push_back(r.smoothUs);
		}
		const float n = rs.empty() ? 1.f : static_cast<float>(rs.size());
		printf("%-14s | %6.1f | %6d / %-6d | %6d , %6.3f | %13.3f | %6.3f / %6.3f | %10.1f / %8.1f / %8.2f\n",
			modes[m].name, 100.f * arrived / n, stuckPairs, stuckTotal, offPairs, offMax,
			ratioSum / n, wallMin, wallSum / n, Median(times), Median(astar), Median(smooth));
	}

	// CSV
	SYSTEMTIME st; ::GetLocalTime(&st);
	wchar_t name[64];
	swprintf_s(name, L"compare_%04d%02d%02d_%02d%02d%02d.csv", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	const wstring csv = cfg.outDir + name;
	std::ofstream f(csv, std::ios::trunc);
	f << "mode,pairId,set,arrived,stuck,offMeshDepth,pathLen,lenRatio,waypoints,minWall,findPath_us,astar_us,smooth_us,simTime,moveLen,maxTurnDeg,sharpTurns\n" << std::fixed;
	for (int m = 0; m < MODE_COUNT; ++m)
		for (size_t i = 0; i < results[m].size(); ++i)
		{
			const Result& r = results[m][i];
			f << modes[m].name << ',' << i << ',' << pairs[i].set << ',' << r.arrived << ',' << r.stuck << ','
				<< std::setprecision(3) << r.offMeshDepth << ',' << r.pathLen << ',' << r.lenRatio << ',' << r.waypoints << ','
				<< r.minWall << ',' << std::setprecision(1) << r.findPathUs << ',' << r.astarUs << ',' << std::setprecision(2) << r.smoothUs << ','
				<< r.simTime << ',' << std::setprecision(3) << r.moveLen << ',' << std::setprecision(1) << r.maxTurnDeg << ',' << r.sharpTurns << '\n';
		}
	wprintf(L"[Benchmark] csv: %s\n", csv.c_str());
}

bool NavMeshBenchmark::AppendCornerPair(const wstring& path, const Vec3& s, const Vec3& e)
{
	const size_t pos = path.find_last_of(L"\\/");
	if (pos != wstring::npos) EnsureDir(path.substr(0, pos + 1));
	std::ofstream f(path, std::ios::app);
	if (!f.is_open()) return false;
	WritePair(f, "corner", s, e);
	printf("[Benchmark] corner pair saved (%.2f, %.2f) -> (%.2f, %.2f)\n", s.x, s.z, e.x, e.z);
	return true;
}
