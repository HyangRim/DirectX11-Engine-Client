#include "pch.h"
#include "FrameProfiler.h"
#include <algorithm>
#include <fstream>
#include <iomanip>

bool FrameProfiler::s_active = false;

namespace
{
    void EnsureDirP(const wstring& dir)
    {
        for (size_t pos = dir.find_first_of(L"\\/"); pos != wstring::npos; pos = dir.find_first_of(L"\\/", pos + 1))
        {
            const wstring sub = dir.substr(0, pos);
            if (!sub.empty() && sub != L"..") ::CreateDirectoryW(sub.c_str(), nullptr);
        }
    }

    struct Stat { double mean = 0, median = 0, p95 = 0, max = 0, calls = 0; };
    Stat Make(const vector<float>& v, const vector<int>& c)
    {
        Stat s;
        if (v.empty()) return s;
        vector<float> t(v);
        std::sort(t.begin(), t.end());
        double sum = 0; for (float x : v) sum += x;
        long long cs = 0; for (int x : c) cs += x;
        s.mean = sum / v.size() / 1000.0;
        s.median = t[t.size() / 2] / 1000.0;
        s.p95 = t[min(t.size() - 1, static_cast<size_t>(t.size() * 0.95))] / 1000.0;
        s.max = t.back() / 1000.0;
        s.calls = static_cast<double>(cs) / v.size();
        return s;
    }
}

FrameProfiler& FrameProfiler::Get()
{
    static FrameProfiler s;
    return s;
}

void FrameProfiler::Start(int frames, const string& note)
{
    m_index.clear();
    m_sections.clear();
    m_frameUs.assign(max(1, frames), 0.f);
    m_target = max(1, frames);
    m_frame = 0;
    m_inFrame = false;
    m_justFinished = false;
    m_note = note;
    s_active = true;
    printf("[Profiler] start : %d frames (%s)\n", m_target, note.c_str());
}

void FrameProfiler::Cancel()
{
    s_active = false;
    m_inFrame = false;
}

bool FrameProfiler::JustFinished()
{
    const bool r = m_justFinished;
    m_justFinished = false;
    return r;
}

void FrameProfiler::BeginFrame()
{
    if (!s_active) return;
    m_inFrame = true;
    m_frameStart = std::chrono::high_resolution_clock::now();
}

void FrameProfiler::Add(const char* category, const char* name, double us, int calls)
{
    if (!s_active || !m_inFrame) return;
    const auto key = make_pair(category, name);
    auto it = m_index.find(key);
    int idx;
    if (it == m_index.end())
    {
        idx = static_cast<int>(m_sections.size());
        Section s;
        s.category = category ? category : "";
        s.name = name ? name : "";
        s.us.assign(m_target, 0.f);
        s.calls.assign(m_target, 0);
        m_sections.push_back(std::move(s));
        m_index.emplace(key, idx);
    }
    else idx = it->second;
    m_sections[idx].us[m_frame] += static_cast<float>(us);
    m_sections[idx].calls[m_frame] += calls;
}

void FrameProfiler::EndFrame()
{
    if (!s_active || !m_inFrame) return;
    m_frameUs[m_frame] = static_cast<float>(std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - m_frameStart).count());
    m_inFrame = false;
    if (++m_frame >= m_target)
        Finish();
}

void FrameProfiler::Finish()
{
    s_active = false;
    m_justFinished = true;

    EnsureDirP(outDir);
    SYSTEMTIME st; ::GetLocalTime(&st);
    wchar_t stamp[64];
    swprintf_s(stamp, L"profile_%04d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    const wstring base = outDir + stamp;

    // 섹션 통계 (평균 큰 순)
    struct Row { const Section* s; Stat st; };
    vector<Row> rows;
    for (const Section& s : m_sections) rows.push_back({ &s, Make(s.us, s.calls) });
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) { return a.st.mean > b.st.mean; });
    const Stat frame = Make(m_frameUs, vector<int>(m_frameUs.size(), 1));

    {
        std::ofstream f(base + L"_summary.csv", std::ios::trunc);
        f << "category,name,mean_ms,median_ms,p95_ms,max_ms,calls_per_frame\n" << std::fixed << std::setprecision(4);
        f << "Frame,total," << frame.mean << ',' << frame.median << ',' << frame.p95 << ',' << frame.max << ",1\n";
        for (const Row& r : rows)
            f << r.s->category << ',' << r.s->name << ',' << r.st.mean << ',' << r.st.median << ',' << r.st.p95 << ',' << r.st.max << ',' << r.st.calls << '\n';
    }
    {
        // 프레임별 : 전체 + 평균 상위 15개 구간
        std::ofstream f(base + L"_frames.csv", std::ios::trunc);
        const size_t top = min<size_t>(15, rows.size());
        f << "frame,frame_us";
        for (size_t i = 0; i < top; ++i) f << ',' << rows[i].s->category << '/' << rows[i].s->name;
        f << '\n' << std::fixed << std::setprecision(1);
        for (int k = 0; k < m_target; ++k)
        {
            f << k << ',' << m_frameUs[k];
            for (size_t i = 0; i < top; ++i) f << ',' << rows[i].s->us[k];
            f << '\n';
        }
    }
    {
        std::ofstream f(base + L"_summary.txt", std::ios::trunc);
        f << "[Profiler] " << m_target << " frames  " << m_note << "\n";
        f << std::fixed << std::setprecision(3);
        f << "Frame total (incl. Present/vsync)  mean " << frame.mean << " ms  median " << frame.median << "  p95 " << frame.p95 << "  max " << frame.max << "\n\n";
        f << std::left << std::setw(14) << "category" << std::setw(34) << "name" << std::right
          << std::setw(10) << "mean ms" << std::setw(10) << "median" << std::setw(10) << "p95" << std::setw(10) << "max" << std::setw(10) << "calls" << "\n";
        for (const Row& r : rows)
        {
            f << std::left << std::setw(14) << r.s->category << std::setw(34) << r.s->name << std::right
              << std::setw(10) << r.st.mean << std::setw(10) << r.st.median << std::setw(10) << r.st.p95 << std::setw(10) << r.st.max
              << std::setw(10) << std::setprecision(1) << r.st.calls << std::setprecision(3) << "\n";
        }
    }

    printf("\n[Profiler] %d frames, frame mean %.2f ms (p95 %.2f)  %s\n", m_target, frame.mean, frame.p95, m_note.c_str());
    const size_t show = min<size_t>(12, rows.size());
    for (size_t i = 0; i < show; ++i)
        printf("  %-12s %-32s %8.3f ms  (calls %.0f)\n", rows[i].s->category.c_str(), rows[i].s->name.c_str(), rows[i].st.mean, rows[i].st.calls);
    wprintf(L"[Profiler] files: %s_summary.txt / _summary.csv / _frames.csv\n\n", base.c_str());
}
