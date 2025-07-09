#pragma once
#include "pch.h"

class GameObject;
class Camera;

struct QuadTreeNode 
{
    RECT bounds;
    vector<shared_ptr<GameObject>> objects;
    unique_ptr<QuadTreeNode> children[4];
    bool isLeaf = true;

    static const int MAX_OBJECTS = 5;
    static const int MAX_DEPTH = 10;

    // 소멸자 선언
    ~QuadTreeNode();
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

    // 디버그 함수
    void PrintDuplicates();



    /////////디벅
    
public:
    // 디버그 함수들
    void DebugDraw(shared_ptr<Camera> camera);
    void PrintTreeStructure();
    void GetNodeBounds(vector<RECT>& bounds, vector<int>& depths);

private:
    // 디버그 헬퍼 함수들
    void DebugDrawNode(const unique_ptr<QuadTreeNode>& node, int depth, shared_ptr<Camera> camera);
    void PrintNodeStructure(const unique_ptr<QuadTreeNode>& node, int depth, const string& prefix);
    void CollectNodeBounds(const unique_ptr<QuadTreeNode>& node, int depth,
        vector<RECT>& bounds, vector<int>& depths);
    /////////////////////////////////
    string ws2s(const wstring& wstr);

public:
    void Split(unique_ptr<QuadTreeNode>& node, int depth);
    void InsertIntoNode(unique_ptr<QuadTreeNode>& node, shared_ptr<GameObject> object, int depth);
    void QueryNode(const unique_ptr<QuadTreeNode>& node, const Ray& ray, shared_ptr<Camera> camera,
        vector<shared_ptr<GameObject>>& result);
    bool RayIntersectsRect(const Ray& ray, const RECT& rect, shared_ptr<Camera> camera);
    RECT GetObjectScreenBounds(shared_ptr<GameObject> object, shared_ptr<Camera> camera);

    // 중복 방지 함수들
    int FindBestChild(const unique_ptr<QuadTreeNode>& node, const RECT& objBounds);
    bool Contains(shared_ptr<GameObject> object);
    bool ContainsInNode(const unique_ptr<QuadTreeNode>& node, shared_ptr<GameObject> object);

    // 디버그 함수
    void CountObjectsInNode(const unique_ptr<QuadTreeNode>& node,
        unordered_map<shared_ptr<GameObject>, int>& objectCount);

    
    unique_ptr<QuadTreeNode> m_root;
    float m_screenWidth;
    float m_screenHeight;
    unordered_set<shared_ptr<GameObject>> m_insertedObjects; // 중복 방지용
};
