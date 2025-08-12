#pragma once

enum class ITEMTYPE {
    EQUIPABLE,
    CONSUMABLE,  // 소비템
    MATERIAL,    // 제작 재료
};

enum class ITEMGRADE {
    COMMON,      // 일반 (흰색)
    UNCOMMON,    // 고급 (초록색)
    RARE,        // 희귀 (파란색)
    EPIC,        // 영웅 (보라색)
    LEGENDARY    // 전설 (주황색)
};

class Item
{
public:
    Item();
    virtual ~Item();

public:
    // 기본 정보
    void SetName(const wstring& _name) { m_itemName = _name; }
    void SetName(wstring&& _name) { m_itemName = move(_name); }
    const wstring& GetName() const { return m_itemName; }

    void SetDescription(const wstring& _desc) { m_itemDescription = _desc; }
    void SetDescription(wstring&& _desc) { m_itemDescription = move(_desc); }
    const wstring& GetDesc() const { return m_itemDescription; }

    // 아이템 타입/등급 - inline으로 최적화
    void SetItemType(ITEMTYPE _type) noexcept { m_itemType = _type; }
    ITEMTYPE GetItemType() const noexcept { return m_itemType; }

    void SetItemGrade(ITEMGRADE _grade) noexcept { m_itemGrade = _grade; }
    ITEMGRADE GetItemGrade() const noexcept { return m_itemGrade; }

    void SetItemID(int32 _id) noexcept { m_itemID = _id; }
    int32 GetItemID() const noexcept { return m_itemID; }

    // 이미지
    void SetMaterial(shared_ptr<Material> _itemMaterial) { m_itemMaterial = _itemMaterial; }
    shared_ptr<Material> GetMaterial() const { return m_itemMaterial; }

    // 이미지 - 텍스처로 변경하여 메모리 최적화
    void SetImage(shared_ptr<Texture> _itemTexture) { m_itemTexture = _itemTexture; }
    shared_ptr<Texture> GetImage() const { return m_itemTexture; }

    // 가상 함수들 (하위 클래스에서 구현)
    virtual bool Use() { return false; }
    virtual bool CanUse() const { return false; }
    virtual Item* Clone() const = 0;

protected:
    // 기본 속성
    wstring m_itemName;
    wstring m_itemDescription;
    int32 m_itemID = 0;

    ITEMTYPE m_itemType = ITEMTYPE::MATERIAL;
    ITEMGRADE m_itemGrade = ITEMGRADE::COMMON;

    // UI
    shared_ptr<Material> m_itemMaterial;
    shared_ptr<Texture> m_itemTexture;
};