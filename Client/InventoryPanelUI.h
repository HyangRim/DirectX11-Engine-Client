#pragma once

#include "IBasePanelUI.h"

class ItemSlot;
class Item;

class InventoryPanelUI :
    public IBasePanelUI
{
public:
    InventoryPanelUI();
    ~InventoryPanelUI();

    virtual void Initialize();
    virtual void Update();
    virtual void SetVisible(bool visible);
    virtual void Cleanup();

public:
    void CreateInventorySlots();
    
    //임시용
    void CreateTestItems();
 

public:
    vector<shared_ptr<ItemSlot>>& GetInventroySlots() { return m_inventorySlots; }

protected:
    virtual void LoadResources();
    virtual void CreatePanels();

    virtual void RegisterUIObject(shared_ptr<GameObject> uiObject);


private:
    vector<shared_ptr<ItemSlot>> m_inventorySlots;
    vector<shared_ptr<Item>> m_testItems; // 테스트용 아이템들

};


