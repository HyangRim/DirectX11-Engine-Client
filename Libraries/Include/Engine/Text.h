#pragma once
#include "Component.h"

class Material;
class Mesh;
class Texture;

class Text : public Component
{
    using Super = Component;

public:
    Text();
    virtual ~Text();

    virtual void Init() override;
    virtual void Update() override;

    // 텍스트 설정 함수들
    void SetText(const wstring& text);
    void SetColor(const Vec4& color);
    void SetPosition(const Vec2& position);
    void SetFontSize(float size);
    void SetFontName(const wstring& fontName);
    void SetAlpha(float alpha);

    // 외곽선 관련 함수들
    void SetOutlineColor(const Vec4& color);
    void SetOutlineWidth(float width);
    void EnableOutline(bool enable);

    // Getter 함수들
    const wstring& GetText() const { return m_text; }
    const Vec4& GetColor() const { return m_color; }
    const Vec4& GetOutlineColor() const { return m_outlineColor; }
    const Vec2& GetPosition() const { return m_position; }
    float GetFontSize() const { return m_fontSize; }
    float GetAlpha() const { return m_alpha; }
    float GetOutlineWidth() const { return m_outlineWidth; }
    bool IsOutlineEnabled() const { return m_enableOutline; }

    // 간단한 생성 함수
    void Create(Vec2 screenPos, const wstring& text, float fontSize = 16.0f,
        Vec4 color = Vec4(0, 0, 0, 1), float alpha = 1.0f,
        Vec4 outlineColor = Vec4(1, 1, 1, 1), float outlineWidth = 1.0f);

private:
    void CreateTextTexture();
    void UpdateMaterial();
    void PushTextData();

private:
    wstring m_text = L"Text";
    wstring m_fontName = L"Liberation Sans";
    Vec4 m_color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);        // 기본 검은색
    Vec4 m_outlineColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f); // 기본 흰색 외곽선
    Vec2 m_position = Vec2(0.0f, 0.0f);
    float m_fontSize = 16.0f;
    float m_alpha = 1.0f;
    float m_outlineWidth = 1.0f;
    bool m_enableOutline = true;

    bool m_needUpdate = true;

    shared_ptr<Texture> m_textTexture;
    shared_ptr<Material> m_material;
    shared_ptr<Mesh> m_mesh;

    // 텍스트 크기 정보
    int m_textWidth = 0;
    int m_textHeight = 0;

    // 셰이더 변수들
    ComPtr<ID3DX11EffectVectorVariable> m_textColorEffect;
    ComPtr<ID3DX11EffectVectorVariable> m_outlineColorEffect;
    ComPtr<ID3DX11EffectScalarVariable> m_textAlphaEffect;
    ComPtr<ID3DX11EffectScalarVariable> m_outlineWidthEffect;
};
