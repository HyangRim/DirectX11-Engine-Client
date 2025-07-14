#pragma once
#include "Component.h"

class Material;
class Mesh;
class Texture;

class TextButton : public Component
{
    using Super = Component;

public:
    TextButton();
    virtual ~TextButton();

    virtual void Init() override;
    virtual void Update() override;

    // 버튼 기능
    bool Picked(POINT screenPos);
    void AddOnClickedEvent(std::function<void(void)> func);
    void InvokeOnClicked();

    // 텍스트 기능
    void SetText(const wstring& text);
    void SetTextColor(const Vec4& color);
    void SetFontSize(float size);
    void SetFontName(const wstring& fontName);
    void SetAlpha(float alpha);

    // 외곽선 기능
    void SetOutlineColor(const Vec4& color);
    void SetOutlineWidth(float width);
    void EnableOutline(bool enable);

    // 배경 기능
    void SetBackgroundColor(const Vec4& color);
    void SetBackgroundTexture(shared_ptr<Texture> texture);

    // 통합 생성 함수
    void Create(Vec2 screenPos, Vec2 size, const wstring& text,
        float fontSize = 16.0f, Vec4 textColor = Vec4(1, 1, 1, 1),
        Vec4 backgroundColor = Vec4(0.2f, 0.5f, 0.8f, 1));

    // Getter 함수들
    const wstring& GetText() const { return m_text; }
    const Vec4& GetTextColor() const { return m_textColor; }
    const Vec4& GetBackgroundColor() const { return m_backgroundColor; }
    const Vec2& GetSize() const { return m_buttonSize; }
    float GetFontSize() const { return m_fontSize; }

private:
    void CreateTextTexture();
    void CreateBackgroundTexture();
    void UpdateMaterial();
    void CreateButtonMesh(Vec2 size);
    void UpdateTransform();

private:
    // 버튼 관련
    std::function<void(void)> m_onClicked;
    RECT m_rect;
    Vec2 m_buttonSize = Vec2(200, 50);
    Vec2 m_screenPos = Vec2(0, 0);

    // 텍스트 관련
    wstring m_text = L"Button";
    wstring m_fontName = L"Ink Free";
    Vec4 m_textColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float m_fontSize = 16.0f;
    float m_alpha = 1.0f;
    bool m_needTextUpdate = true;

    // 외곽선 관련
    Vec4 m_outlineColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float m_outlineWidth = 1.0f;
    bool m_enableOutline = true;

    // 배경 관련
    Vec4 m_backgroundColor = Vec4(0.2f, 0.5f, 0.8f, 1.0f);
    shared_ptr<Texture> m_backgroundTexture;
    bool m_needBackgroundUpdate = true;

    // 렌더링 관련
    shared_ptr<Texture> m_textTexture;
    shared_ptr<Texture> m_finalTexture;
    shared_ptr<Material> m_material;
    shared_ptr<Mesh> m_mesh;

    // 텍스트 크기 정보
    int m_textWidth = 0;
    int m_textHeight = 0;
};
