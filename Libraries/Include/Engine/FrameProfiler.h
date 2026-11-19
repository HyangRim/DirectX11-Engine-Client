#pragma once
#include <chrono>
#include <typeinfo>
#include <cstring>

// =============================================================================
// FrameProfiler : 프레임 구간별 CPU 시간 측정
//  - PROFILE_SCOPE("분류", "이름") 을 둔 구간의 시간을 프레임마다 합산 (중첩된 구간은 각자 포함 시간)
//  - 이름에 typeid(*obj).name() 처럼 수명이 긴 문자열 포인터를 넘기면 타입별로 나뉜다
//  - Start(N) 후 N 프레임이 지나면 ..\tmp\benchmark\profile_*.csv / *_summary.txt 를 남기고 끝난다
//  - 측정 중이 아니면 bool 하나만 보고 바로 빠진다
// =============================================================================
class FrameProfiler
{
public:
    static FrameProfiler& Get();
    static bool s_active;

    void Start(int frames, const string& note);
    void Cancel();
    bool IsActive() const { return s_active; }
    bool JustFinished();                // 직전 EndFrame 에서 끝났으면 true (한 번만)

    void BeginFrame();
    void EndFrame();
    void Add(const char* category, const char* name, double us, int calls = 1);
    void SetNote(const string& note) { m_note = note; }

    wstring outDir = L"..\\tmp\\benchmark\\";

private:
    struct Section
    {
        string category;
        string name;
        vector<float> us;       // 프레임별 합
        vector<int> calls;      // 프레임별 호출 수
    };
    struct KeyHash
    {
        size_t operator()(const pair<const char*, const char*>& k) const
        {
            return std::hash<const void*>()(k.first) * 31u ^ std::hash<const void*>()(k.second);
        }
    };

    void Finish();

    unordered_map<pair<const char*, const char*>, int, KeyHash> m_index;
    vector<Section> m_sections;
    vector<float> m_frameUs;
    int m_target = 0;
    int m_frame = 0;
    bool m_inFrame = false;
    bool m_justFinished = false;
    string m_note;
    std::chrono::high_resolution_clock::time_point m_frameStart;
};

class ProfileScope
{
public:
    ProfileScope(const char* category, const char* name)
    {
        if (!FrameProfiler::s_active) return;
        m_category = category; m_name = name; m_on = true;
        m_t0 = std::chrono::high_resolution_clock::now();
    }
    ~ProfileScope()
    {
        if (!m_on) return;
        const double us = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - m_t0).count();
        FrameProfiler::Get().Add(m_category, m_name, us);
    }
private:
    const char* m_category = nullptr;
    const char* m_name = nullptr;
    bool m_on = false;
    std::chrono::high_resolution_clock::time_point m_t0;
};

#define PROFILE_CONCAT_INNER(a, b) a##b
#define PROFILE_CONCAT(a, b) PROFILE_CONCAT_INNER(a, b)
#define PROFILE_SCOPE(category, name) ProfileScope PROFILE_CONCAT(_profileScope, __LINE__)(category, name)

// typeid 이름에서 "class " / "struct " 를 뗀 포인터 (원래 문자열 안을 가리키므로 수명이 길다)
inline const char* ProfileTypeName(const std::type_info& t)
{
    const char* n = t.name();
    if (strncmp(n, "class ", 6) == 0) return n + 6;
    if (strncmp(n, "struct ", 7) == 0) return n + 7;
    return n;
}
