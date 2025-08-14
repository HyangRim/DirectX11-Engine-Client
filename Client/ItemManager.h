#pragma once

#include "Item.h"
#include "EquipableItem.h"
#include "IngredientItem.h"

struct ItemData {
    wstring name;
    wstring description;
    ITEMTYPE itemType;
    ITEMGRADE itemGrade;
    wstring iconPath;
    // 장비 아이템인 경우 추가 데이터
    EquipmentType equipType = EquipmentType::WEAPON;
    ItemStatus status;
};

class ItemManager
{
    DECLARE_SINGLE(ItemManager);
    ~ItemManager();

public:
    void Initialize();

    //아이템 아이콘 Material불러오기
    void LoadResources();
    void CreatItems();

    shared_ptr<Item> GetItem(const wstring& name);
   
private:
    unordered_map<wstring, shared_ptr<Item>> m_items;


   
    void LoadItemData();
    void SetupItemRecipes();
};

