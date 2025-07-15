#pragma once

class CollisionManager
{
public:
    CollisionManager();
    ~CollisionManager();

    void Initialize();
    void CheckCollisions(const unordered_set<shared_ptr<GameObject>>& objects);
    void CheckCollisionsWithQuadTree(QuadTree* quadTree, shared_ptr<Camera> camera);
    void ClearCollisionInfo();

private:
    void ProcessCollisionPair(shared_ptr<BaseCollider> collider1,
        shared_ptr<BaseCollider> collider2);
    void UpdateCollisionState(ULONG64 id, bool isColliding,
        shared_ptr<GameObject> obj1, shared_ptr<GameObject> obj2);

    unordered_map<ULONG64, bool> m_collisionMap;
};
