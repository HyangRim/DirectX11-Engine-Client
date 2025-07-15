#pragma once

class ObjectManager
{
public:
    ObjectManager();
    ~ObjectManager();



    void Add(shared_ptr<GameObject> object);
    void Remove(shared_ptr<GameObject> object);
    void MarkForDestroy(shared_ptr<GameObject> object);
    void ProcessPendingDestroy();

    void UpdateAll();
    void FixedUpdateAll();
    void LateUpdateAll();
    void StartAll();

    const unordered_set<shared_ptr<GameObject>>& GetObjects() const { return m_objects; }

private:
    void ProcessDestruction();
    void NotifyDestroy(shared_ptr<GameObject> object);

    unordered_set<shared_ptr<GameObject>> m_objects;
    vector<shared_ptr<GameObject>> m_pendingDestroy;
    bool m_isDestroying = false;
};
