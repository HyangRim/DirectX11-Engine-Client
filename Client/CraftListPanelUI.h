#pragma once
#include "IBasePanelUI.h"

class Player;
class Recipe;
class ItemSlot;

class CraftListPanelUI :
    public IBasePanelUI
{
public:
    CraftListPanelUI(shared_ptr<Player> player);
    virtual ~CraftListPanelUI();

    virtual void Initialize();
    virtual void Update();
    virtual void SetVisible(bool visible);
    virtual void Cleanup();

private:
    void UpdateCraftableItems();
    void CreateCraftSlots();
    void ClearCraftSlots();
    void OnCraftSlotClicked(int slotIndex);

protected:
    virtual void LoadResources();
    virtual void CreatePanels();

    virtual void RegisterUIObject(shared_ptr<GameObject> uiObject);

private:
    shared_ptr<Player> m_player;
    // 제작 가능한 레시피들
    vector<shared_ptr<Recipe>> m_craftableRecipes;

    // 제작 슬롯들
    vector<shared_ptr<ItemSlot>> m_craftSlots;

    // UI 관련
    shared_ptr<UIPanel> m_scrollPanel;

    
};

