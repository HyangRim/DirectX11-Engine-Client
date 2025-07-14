#pragma once
#include "Component.h"

class Material;
class Mesh;
class Texture;

struct ImageLayer
{
    shared_ptr<Texture> texture;
    int layer = 0;
    Vec4 tint = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float alpha = 1.0f;
    Vec2 offset = Vec2(0.0f, 0.0f);
    Vec2 scale = Vec2(1.0f, 1.0f);
    bool visible = true;

    ImageLayer() = default;
    ImageLayer(shared_ptr<Texture> tex, int layerIndex = 0)
        : texture(tex), layer(layerIndex) {
    }
};

class LayeredImageRenderer : public Component
{
    using Super = Component;

public:
    LayeredImageRenderer();
    virtual ~LayeredImageRenderer();

    virtual void Init() override;
    virtual void Update() override;

    // 레이어 관리
    void AddLayer(shared_ptr<Texture> texture, int layer = 0);
    void AddLayer(const ImageLayer& imageLayer);
    void RemoveLayer(int layer);
    void ClearLayers();

    // 레이어 속성 설정
    void SetLayerTint(int layer, const Vec4& tint);
    void SetLayerAlpha(int layer, float alpha);
    void SetLayerOffset(int layer, const Vec2& offset);
    void SetLayerScale(int layer, const Vec2& scale);
    void SetLayerVisible(int layer, bool visible);

    // 전체 속성
    void SetPosition(const Vec2& position);
    void SetSize(const Vec2& size);
    void SetAlpha(float alpha);

    // Getter
    const vector<ImageLayer>& GetLayers() const { return m_layers; }
    ImageLayer* GetLayer(int layer);
    const Vec2& GetPosition() const { return m_position; }
    const Vec2& GetSize() const { return m_size; }
    float GetAlpha() const { return m_alpha; }

private:
    void SortLayers();
    void CreateCombinedTexture();
    void UpdateMaterial();
    void UpdateTransform();

    // 텍스처 합성 함수들
    vector<BYTE> ExtractTextureData(shared_ptr<Texture> texture);
    void BlendLayers(vector<BYTE>& destBuffer, const vector<BYTE>& srcBuffer,
        int width, int height, const ImageLayer& layer);
    void ApplyLayerTransform(vector<BYTE>& buffer, int width, int height,
        const ImageLayer& layer);

private:
    vector<ImageLayer> m_layers;
    Vec2 m_position = Vec2(0.0f, 0.0f);
    Vec2 m_size = Vec2(100.0f, 100.0f);
    float m_alpha = 1.0f;

    bool m_needsUpdate = true;

    shared_ptr<Texture> m_combinedTexture;
    shared_ptr<Material> m_material;
    shared_ptr<Mesh> m_mesh;

    int m_finalWidth = 256;
    int m_finalHeight = 256;
};
