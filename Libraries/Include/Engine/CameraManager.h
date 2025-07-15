#pragma once
#include <memory>
#include <unordered_set>

class GameObject;
class Camera;
class Sky;

class CameraManager
{
public:
    void AddCamera(shared_ptr<GameObject> camera);
    void RemoveCamera(shared_ptr<GameObject> camera);

    shared_ptr<GameObject> GetMainCamera();
    shared_ptr<GameObject> GetUICamera();
    shared_ptr<GameObject> GetLight() { return m_light; }  // Ãß°¡

    void RenderAll();
    void SetSky(shared_ptr<Sky> sky);
    void SetLight(shared_ptr<GameObject> light);

private:
    void RenderGameCamera(Camera* cam);
    void RenderUICamera(Camera* cam);

    unordered_set<shared_ptr<GameObject>> m_cameras;
    shared_ptr<Sky> m_sky;
    shared_ptr<GameObject> m_light;
};
