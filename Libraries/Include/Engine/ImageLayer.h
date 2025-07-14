#pragma once
#include "Component.h"

struct LayerInfo {
    shared_ptr<class Material> material;
    shared_ptr<class Mesh> mesh;
    Vec3 offset = Vec3::Zero;  // 레이어별 위치 오프셋
    Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);  // 레이어별 크기
    float alpha = 1.0f;  // 레이어별 투명도
    bool visible = true;  // 레이어별 가시성
    int renderOrder = 0;  // 렌더링 순서 (낮을수록 먼저 그려짐)
};

class ImageLayer : public Component
{
    using Super = Component;
public:
    ImageLayer();
    virtual ~ImageLayer();

    virtual void Update() override;

    // 기본 생성 함수
    void Create(Vec2 screenPos, Vec2 size);

    // 레이어 관리 함수들
    int AddLayer(shared_ptr<class Material> material,
        Vec3 offset = Vec3::Zero,
        Vec3 scale = Vec3(1.0f, 1.0f, 1.0f),
        float alpha = 1.0f,
        int renderOrder = 0);

    void RemoveLayer(int layerIndex);
    void ClearAllLayers();

    // 레이어 속성 수정 함수들
    void SetLayerOffset(int layerIndex, const Vec3& offset);
    void SetLayerScale(int layerIndex, const Vec3& scale);
    void SetLayerAlpha(int layerIndex, float alpha);
    void SetLayerVisible(int layerIndex, bool visible);
    void SetLayerRenderOrder(int layerIndex, int renderOrder);
    void SetLayerMaterial(int layerIndex, shared_ptr<class Material> material);

    // 전체 이미지 속성 함수들
    void SetPosition(Vec2 screenPos);
    void SetSize(Vec2 size);
    void SetVisible(bool visible);

    // Getter 함수들
    int GetLayerCount() const { return static_cast<int>(m_layers.size()); }
    const LayerInfo* GetLayerInfo(int layerIndex) const;
    Vec2 GetPosition() const { return m_screenPos; }
    Vec2 GetSize() const { return m_size; }
    bool IsVisible() const { return m_visible; }

private:
    void UpdateTransform();
    void SortLayersByRenderOrder();
    void RenderLayer(const LayerInfo& layer);

private:
    Vec2 m_screenPos = Vec2::Zero;
    Vec2 m_size = Vec2(100.0f, 100.0f);
    bool m_visible = true;

    vector<LayerInfo> m_layers;
    int m_nextLayerIndex = 0;

    // 렌더링용 기본 메시
    shared_ptr<class Mesh> m_quadMesh;
};
