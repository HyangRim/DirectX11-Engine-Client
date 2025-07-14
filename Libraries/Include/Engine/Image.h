#pragma once
#include "Component.h"

class Material;
class Mesh;
class Texture;

class ImageRenderer : public Component
{
    using Super = Component;

public:
    ImageRenderer();
    virtual ~ImageRenderer();

    virtual void Init() override;
    virtual void Update() override;

    // 이미지 설정 함수들
    void SetTexture(shared_ptr<Texture> texture);
    void SetPosition(const Vec2& position);
    void SetSize(const Vec2& size);
    void SetAlpha(float alpha);
    void SetColor(const Vec4& color);

    // Getter 함수들
    shared_ptr<Texture> GetTexture() const { return m_texture; }
    const Vec2& GetPosition() const { return m_position; }
    const Vec2& GetSize() const { return m_size; }
    float GetAlpha() const { return m_alpha; }
    const Vec4& GetColor() const { return m_color; }

    // 간단한 생성 함수
    void Create(Vec2 screenPos, shared_ptr<Texture> texture, Vec2 size = Vec2(100, 100), float alpha = 1.0f);

private:
    void UpdateMaterial();
    void UpdateTransform();

private:
    shared_ptr<Texture> m_texture;
    shared_ptr<Material> m_material;
    shared_ptr<Mesh> m_mesh;

    Vec2 m_position = Vec2(0.0f, 0.0f);
    Vec2 m_size = Vec2(100.0f, 100.0f);
    float m_alpha = 1.0f;
    Vec4 m_color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

    bool m_needUpdate = true;
};
