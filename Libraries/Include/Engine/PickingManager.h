#pragma once
#include <memory>
#include <vector>
#include <unordered_set>
#include <string>

class GameObject;
class Camera;
class QuadTree;

class PickingManager
{
public:
    shared_ptr<GameObject> PickObjectOrUI(const POINT& screenPt,
        shared_ptr<Camera> mainCamera,
        shared_ptr<Camera> uiCamera,
        QuadTree* quadTree,
        const unordered_set<shared_ptr<GameObject>>& uiObjects);

private:
    bool HandleUIClick(const POINT& screenPt, shared_ptr<Camera> uiCamera,
        const unordered_set<shared_ptr<GameObject>>& uiObjects);

    shared_ptr<GameObject> PickGameObject(const POINT& screenPt,
        shared_ptr<Camera> mainCamera,
        QuadTree* quadTree);

    shared_ptr<GameObject> PickTerrain(const POINT& screenPt,
        shared_ptr<Camera> mainCamera,
        const vector<shared_ptr<GameObject>>& objects,
        float& minDistance);

    Ray CreateRayFromScreen(const Vec2& screenPos, shared_ptr<Camera> camera);

    vector<shared_ptr<GameObject>> FilterValidCandidates(
        const vector<shared_ptr<GameObject>>& candidates,
        shared_ptr<Camera> camera, QuadTree* quadTree);

    string ConvertWStringToString(const wstring& wstr);
};
