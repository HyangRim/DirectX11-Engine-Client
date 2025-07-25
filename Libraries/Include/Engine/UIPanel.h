#pragma once
#include "Component.h"

class Material;
class Mesh;
class Texture;
class Button;
class Text;
class ImageUI;

class UIPanel : public Component
{
    using Super = Component;

public:
    UIPanel();
    virtual ~UIPanel();

    virtual void Init() override;
    virtual void Update() override;

    // 패널 설정 함수들
    void SetPosition(const Vec2& position);
    void SetSize(const Vec2& size);
    void SetBackgroundColor(const Vec4& color);
    void SetBackgroundTexture(shared_ptr<Texture> texture);
    void SetVisible(bool visible);

    // UI 요소 추가 함수들
    shared_ptr<Button> AddButton(Vec2 localPos, Vec2 size, shared_ptr<Material> material, const wstring& name = L"Button");
    shared_ptr<Text> AddText(Vec2 localPos, const wstring& text, float fontSize = 16.0f,
        Vec4 color = Vec4(1, 1, 1, 1), float alpha = 1.0f,
        Vec4 outlineColor = Vec4(0, 0, 0, 1), float outlineWidth = 1.0f,
        const wstring& name = L"Text");
    // ImageUI 추가 함수
    shared_ptr<ImageUI> AddImageUI(Vec2 localPos, const wstring& name = L"ImageUI");

    // UI 요소 관리
    void RemoveUIElement(const wstring& name);
    shared_ptr<Button> GetButton(const wstring& name);
    shared_ptr<Text> GetText(const wstring& name);
    shared_ptr<ImageUI> GetImageUI(const wstring& name);


    // Getter 함수들
    const Vec2& GetPosition() const { return m_position; }
    const Vec2& GetSize() const { return m_size; }
    bool IsVisible() const { return m_visible; }

    // 패널 생성 함수
    void Create(Vec2 screenPos, Vec2 size, shared_ptr<Material> backgroundMaterial = nullptr);

public:
    // 소멸 관련 메서드
    virtual void OnDestroy() override;
    void ClearChildReferences() {
        // weak_ptr은 순환 참조를 만들지 않음
        m_childElements.clear();
        m_namedElements.clear();
    }

public:
    const vector<weak_ptr<GameObject>>& GetChildElements() const { return m_childElements; }

    // 더 안전한 자식 제거 메서드
    void RemoveUIElementSafely(const wstring& name);

private:
    void CreatePanelBackground();
    void UpdateChildPositions();
    Vec2 LocalToWorldPosition(const Vec2& localPos);

private:
    Vec2 m_position = Vec2(0.0f, 0.0f);
    Vec2 m_size = Vec2(200.0f, 150.0f);
    Vec4 m_backgroundColor = Vec4(1.f);
    bool m_visible = true;

    // 배경 렌더링용
    shared_ptr<Texture> m_backgroundTexture;
    shared_ptr<Material> m_backgroundMaterial;
    shared_ptr<Mesh> m_backgroundMesh;

    // 자식 UI 요소들
    vector<weak_ptr<GameObject>> m_childElements;
    map<wstring, weak_ptr<GameObject>> m_namedElements;

private:
    bool m_isDestroying = false;  // 소멸 중 플래그 추가
};