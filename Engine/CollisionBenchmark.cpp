#include "pch.h"
#include "CollisionBenchmark.h"
#include <algorithm>
#include <fstream>
#include <iomanip>

namespace
{
    void EnsureDir(const wstring& dir)
    {
        for (size_t pos = dir.find_first_of(L"\\/"); pos != wstring::npos; pos = dir.find_first_of(L"\\/", pos + 1))
        {
            const wstring sub = dir.substr(0, pos);
            if (!sub.empty() && sub != L"..") ::CreateDirectoryW(sub.c_str(), nullptr);
        }
    }

    struct Dist { double median = 0, p99 = 0, max = 0, mean = 0; };
    Dist Summarize(vector<double> v)
    {
        Dist d;
        if (v.empty()) return d;
        std::sort(v.begin(), v.end());
        auto at = [&](double q) { return v[min(v.size() - 1, static_cast<size_t>(q * (v.size() - 1) + 0.5))]; };
        d.median = at(0.5); d.p99 = at(0.99); d.max = v.back();
        double s = 0; for (double x : v) s += x; d.mean = s / v.size();
        return d;
    }
}

void CollisionBenchmark::Start(int frames, int driver)
{
    m_rows.clear();
    m_rows.reserve(frames);
    m_target = max(1, frames);
    m_recording = true;
    wprintf(L"[CollisionBench] start : %d frames (events by %s)\n", m_target, driver == 0 ? L"ScreenQuadTree" : L"LooseQuadTree");
}

void CollisionBenchmark::Cancel()
{
    m_recording = false;
    m_rows.clear();
}

void CollisionBenchmark::AddRow(const CollisionBenchRow& row)
{
    if (!m_recording) return;
    m_rows.push_back(row);
    if (static_cast<int>(m_rows.size()) >= m_target)
        Finish();
}

void CollisionBenchmark::Finish()
{
    m_recording = false;
    if (m_rows.empty()) return;

    EnsureDir(outDir);
    SYSTEMTIME st; ::GetLocalTime(&st);
    wchar_t name[64];
    swprintf_s(name, L"collision_%04d%02d%02d_%02d%02d%02d.csv", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    const wstring csv = outDir + name;

    std::ofstream f(csv, std::ios::trunc);
    f << "frame,eligible,"
         "screen_inserted,screen_update_us,screen_rebuilt,screen_nodes_created,screen_query_us,screen_narrow,screen_pairs,screen_missed,screen_extra,"
         "loose_sync_us,loose_query_us,loose_inserted,loose_removed,loose_moved,loose_outside,loose_aabb,loose_narrow,loose_pairs,loose_missed,loose_extra,"
         "brute_us,brute_narrow,brute_pairs,driver,driver_stale\n";
    f << std::fixed << std::setprecision(2);
    for (const CollisionBenchRow& r : m_rows)
    {
        f << r.frame << ',' << r.eligible << ','
          << r.screenInserted << ',' << r.screenUpdateUs << ',' << r.screenRebuilt << ',' << r.screenNodesCreated << ','
          << r.screenQueryUs << ',' << r.screenNarrow << ',' << r.screenPairs << ',' << r.screenMissed << ',' << r.screenExtra << ','
          << r.looseSyncUs << ',' << r.looseQueryUs << ',' << r.looseInserted << ',' << r.looseRemoved << ',' << r.looseMoved << ','
          << r.looseOutside << ',' << r.looseAabb << ',' << r.looseNarrow << ',' << r.loosePairs << ',' << r.looseMissed << ',' << r.looseExtra << ','
          << r.bruteUs << ',' << r.bruteNarrow << ',' << r.brutePairs << ',' << r.driver << ',' << r.driverStale << '\n';
    }
    f.close();

    // ---- 콘솔 요약
    vector<double> sTot, sQ, lTot, lQ, bT, sN, lN, bN, elig;
    long long sMiss = 0, sExtra = 0, lMiss = 0, lExtra = 0, stale = 0, rebuilt = 0, nodes = 0, moved = 0, truth = 0;
    for (const CollisionBenchRow& r : m_rows)
    {
        sTot.push_back(r.screenUpdateUs + r.screenQueryUs); sQ.push_back(r.screenQueryUs);
        lTot.push_back(r.looseSyncUs + r.looseQueryUs);     lQ.push_back(r.looseQueryUs);
        bT.push_back(r.bruteUs);
        sN.push_back(static_cast<double>(r.screenNarrow)); lN.push_back(static_cast<double>(r.looseNarrow)); bN.push_back(static_cast<double>(r.bruteNarrow));
        elig.push_back(r.eligible);
        sMiss += r.screenMissed; sExtra += r.screenExtra; lMiss += r.looseMissed; lExtra += r.looseExtra;
        stale += r.driverStale; rebuilt += r.screenRebuilt; nodes += r.screenNodesCreated; moved += r.looseMoved;
        truth += r.brutePairs;
    }
    const Dist dsT = Summarize(sTot), dsQ = Summarize(sQ), dlT = Summarize(lTot), dlQ = Summarize(lQ), db = Summarize(bT);
    const Dist dsN = Summarize(sN), dlN = Summarize(lN), dbN = Summarize(bN), de = Summarize(elig);
    const double n = static_cast<double>(m_rows.size());

    wprintf(L"\n[CollisionBench] %d frames, colliders median %.0f, true pairs total %lld\n", static_cast<int>(m_rows.size()), de.median, truth);
    wprintf(L"  %-16s | %10s %10s %10s | %12s | %8s %8s\n", L"method", L"median us", L"p99 us", L"max us", L"narrow/frm", L"missed", L"extra");
    wprintf(L"  %-16s | %10.1f %10.1f %10.1f | %12.1f | %8s %8s\n", L"BruteForce", db.median, db.p99, db.max, dbN.mean, L"-", L"-");
    wprintf(L"  %-16s | %10.1f %10.1f %10.1f | %12.1f | %8lld %8lld\n", L"Screen (total)", dsT.median, dsT.p99, dsT.max, dsN.mean, sMiss, sExtra);
    wprintf(L"  %-16s | %10.1f %10.1f %10.1f |\n", L"Screen (query)", dsQ.median, dsQ.p99, dsQ.max);
    wprintf(L"  %-16s | %10.1f %10.1f %10.1f | %12.1f | %8lld %8lld\n", L"Loose (total)", dlT.median, dlT.p99, dlT.max, dlN.mean, lMiss, lExtra);
    wprintf(L"  %-16s | %10.1f %10.1f %10.1f |\n", L"Loose (query)", dlQ.median, dlQ.p99, dlQ.max);
    wprintf(L"  screen rebuilt %.1f%% of frames, nodes created %.1f/frame, loose moved %.2f/frame, driver stale %lld\n",
        100.0 * rebuilt / n, nodes / n, moved / n, stale);
    wprintf(L"  (Screen total includes UpdateQuadTree, which FOW / Picking also use)\n");
    wprintf(L"[CollisionBench] csv: %s\n\n", csv.c_str());
}
