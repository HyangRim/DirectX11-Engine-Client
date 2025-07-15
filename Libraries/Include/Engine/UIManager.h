#pragma once

class UIManager
{
public:
    UIManager();
    ~UIManager();

    void AddUIObject(shared_ptr<GameObject> object, bool isParent = false);
    void RemoveUIObject(shared_ptr<GameObject> object);
    void ProcessPendingDestroy();

    void UpdateAll();
    void FixedUpdateAll();
    void LateUpdateAll();
    void StartAll();

    bool HandleUIClick(const POINT& screenPt, shared_ptr<Camera> camera);
    const unordered_set<shared_ptr<GameObject>>& GetUIObjects() const { return m_uiObjects; }

private:
    void ProcessUIDestruction();
    void RegisterUIParent(shared_ptr<GameObject> parent);
    void RegisterUIChild(shared_ptr<GameObject> child);

    unordered_set<shared_ptr<GameObject>> m_uiObjects;
    vector<shared_ptr<GameObject>> m_uiParents;
    vector<shared_ptr<GameObject>> m_uiChildren;
    vector<shared_ptr<GameObject>> m_pendingDestroy;
    bool m_isDestroying = false;
};
