#pragma once
#include "pch.h"
#include <chrono>

class GameObject;
class Camera;

struct QuadTreeNode
{
    RECT bounds;
    vector<shared_ptr<GameObject>> objects;
    unique_ptr<QuadTreeNode> children[4];
    bool isLeaf = true;
    int nodeId = 0; // 디버깅용 노드 ID

    static const int MAX_OBJECTS = 8;  
    static const int MAX_DEPTH = 12;  

    ~QuadTreeNode();
};

// 성능 통계 구조체
struct QuadTreeStats
{
    int totalNodes = 0;
    int leafNodes = 0;
    int totalObjects = 0;
    int maxDepth = 0;
    float avgObjectsPerLeaf = 0.0f;
    std::chrono::microseconds lastQueryTime{ 0 };
    std::chrono::microseconds lastBuildTime{ 0 };
};

class VflyQuadTree
{
public:
    VflyQuadTree(float screenWidth, float screenHeight);
    ~VflyQuadTree();

    void Clear();
    void Insert(shared_ptr<GameObject> object);
    void Build();
    vector<shared_ptr<GameObject>> Query(const Ray& ray, shared_ptr<Camera> camera);

    // 성능 관련
    const QuadTreeStats& GetStats() const { return m_stats; }
    void UpdateStats();

    // 디버그 함수들
    void DebugDraw(shared_ptr<Camera> camera);
    void PrintTreeStructure();
    void PrintDuplicates();
    void GetNodeBounds(vector<RECT>& bounds, vector<int>& depths);

public:
    // 핵심 기능
    void Split(unique_ptr<QuadTreeNode>& node, int depth);
    void InsertIntoNode(unique_ptr<QuadTreeNode>& node, shared_ptr<GameObject> object, int depth);
    void QueryNode(const unique_ptr<QuadTreeNode>& node, const Ray& ray, shared_ptr<Camera> camera,
        vector<shared_ptr<GameObject>>& result);

    // 교차 검사
    bool RayIntersectsAABB(const Ray& ray, const RECT& rect, shared_ptr<Camera> camera);
    RECT GetObjectScreenBounds(shared_ptr<GameObject> object, shared_ptr<Camera> camera);

    // 유틸리티
    bool IsObjectVisible(shared_ptr<GameObject> object, shared_ptr<Camera> camera);
    int FindBestChildren(const unique_ptr<QuadTreeNode>& node, const RECT& objBounds, vector<int>& childrenIndices);

    // 디버그 헬퍼
    void DebugDrawNode(const unique_ptr<QuadTreeNode>& node, int depth, shared_ptr<Camera> camera);
    void PrintNodeStructure(const unique_ptr<QuadTreeNode>& node, int depth, const string& prefix);
    void CollectNodeBounds(const unique_ptr<QuadTreeNode>& node, int depth, vector<RECT>& bounds, vector<int>& depths);
    void CountObjectsInNode(const unique_ptr<QuadTreeNode>& node, unordered_map<shared_ptr<GameObject>, int>& objectCount);
    void CalculateStats(const unique_ptr<QuadTreeNode>& node, int depth);

    string ws2s(const wstring& wstr);
   
    void PrintObjectLocation(shared_ptr<GameObject> targetObject);
    void PrintObjectInNodes(const unique_ptr<QuadTreeNode>& node, shared_ptr<GameObject> targetObject, int depth);

    Vec2 WorldToScreen(const Vec3& worldPos, shared_ptr<Camera> camera);
    Vec3 ScreenToWorld(const Vec2& screenPos, shared_ptr<Camera> camera, float depth = 1.0f);

    void DebugCoordinateTransform(const Vec2& mousePos, shared_ptr<Camera> camera);

private:
    unique_ptr<QuadTreeNode> m_root;
    float m_screenWidth;
    float m_screenHeight;
    unordered_set<shared_ptr<GameObject>> m_insertedObjects;
    QuadTreeStats m_stats;
    static int s_nextNodeId;
};
