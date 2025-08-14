// CraftListPanelUI.cpp
#include "pch.h"
#include "CraftListPanelUI.h"
#include "Player.h"
#include "InventoryManager.h"
#include "RecipeManager.h"
#include "ItemManager.h"
#include "ItemSlot.h"

CraftListPanelUI::CraftListPanelUI(shared_ptr<Player> player)
    : m_player(player)
{
}

CraftListPanelUI::~CraftListPanelUI()
{
}

void CraftListPanelUI::Initialize()
{
    LoadResources();
    CreatePanels();

    // 인벤토리 변화 이벤트 등록
    auto inventoryManager = InventoryManager::GetInstance();
    inventoryManager->OnInventoryChanged.Push([this]() {
        if (m_isVisible) {
            UpdateCraftableItems();
        }
        });


    UpdateCraftableItems();
}

void CraftListPanelUI::Update()
{
    
}

void CraftListPanelUI::SetVisible(bool visible)
{
    m_isVisible = visible;
    if (m_panel)
    {
        m_panel->GetUIPanel()->SetVisible(visible);
    }

    // 보이게 될 때 즉시 업데이트
    if (visible)
    {
        UpdateCraftableItems();
    }
}

void CraftListPanelUI::Cleanup()
{
    ClearCraftSlots();
}

void CraftListPanelUI::LoadResources()
{
    // 필요한 UI 리소스 로드 (이미 ItemManager에서 로드되어 있을 것)
}

void CraftListPanelUI::CreatePanels()
{
    m_panel = make_shared<GameObject>();
    m_panel->SetName(L"CraftListPanel");

    auto panel = make_shared<UIPanel>();
    m_panel->AddComponent(panel);

    // 제작 목록 패널 위치와 크기 설정 (화면 왼쪽)
    Vec2 panelPos = Vec2(150.f, 400.f);
    Vec2 panelSize = Vec2(200.f, 400.f);

    panel->Create(Vec2(960.f, 768 - 57 - 75), Vec2(252, 62), Vec4(1.f, 0.f, 0.f, 1.f), nullptr);

    m_panel->SetLayerIndex(LAYER_UI);

    CURSCENE->AddUIObject(m_panel, true);
    CURSCENE->RegisterUIParent(m_panel);
}

void CraftListPanelUI::UpdateCraftableItems()
{
    // 기존 슬롯들 정리
    ClearCraftSlots();

    // 인벤토리 매니저에서 제작 가능한 레시피들 가져오기
    auto inventoryManager = InventoryManager::GetInstance();
    m_craftableRecipes = inventoryManager->GetAvailableRecipes();

    // 제작 슬롯들 생성
    CreateCraftSlots();
}

void CraftListPanelUI::CreateCraftSlots()
{
    if (!m_panel || m_craftableRecipes.empty())
        return;

    int slotsX = 5;
    int slotsY = 2;
    Vec2 panelSize = m_panel->GetUIPanel()->GetSize();
    Vec2 slotSize = Vec2(46, 28);
    Vec2 spacing = Vec2(5, 5);
    Vec2 startPos = Vec2(960.f - (252 / 2.f) + 23, (768 - 57 - 75) - (62 / 2.f) + 14); // 패널 내 시작 위치

    for (int i = 0; i < m_craftableRecipes.size(); i++)
    {
        auto recipe = m_craftableRecipes[i];
        if (!recipe) continue;

        // 결과 아이템 가져오기
        auto resultItem = ItemManager::GetInstance()->GetItem(recipe->GetResultItemID());
        if (!resultItem) continue;

        // 제작 슬롯 생성
        auto craftSlot = make_shared<ItemSlot>(resultItem, false);
        craftSlot->SetSlotType(SLOTTYPE::INVENTORY); // 제작용 슬롯으로 설정

        Vec2 slotPos = Vec2(
            startPos.x + (i%slotsX) * (slotSize.x + spacing.x),
            startPos.y + (i/slotsY) * (slotSize.y + spacing.y)
        );
        craftSlot->CreateSlot(slotPos, slotSize, i);

        // 클릭 이벤트 등록
        craftSlot->OnSlotClicked.Push([this, i](int slotIndex, SLOTTYPE slotType) {
            OnCraftSlotClicked(i);
            });

        m_craftSlots.push_back(craftSlot);
    }
}

void CraftListPanelUI::ClearCraftSlots()
{
    // 기존 슬롯들 정리
    for (auto& slot : m_craftSlots)
    {
        if (slot)
        {
            slot->ClearItem();
        }
    }
    m_craftSlots.clear();
}

void CraftListPanelUI::OnCraftSlotClicked(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= m_craftableRecipes.size())
        return;

    auto recipe = m_craftableRecipes[slotIndex];
    if (!recipe) return;

    
    // 인벤토리에서 직접 제작 실행
    auto inventoryManager = InventoryManager::GetInstance();
    auto& inventorySlots = inventoryManager->GetInventorySlots();

    if (recipe->ExecuteCraftFromSlots(inventorySlots))
    {
        // 제작 성공 후 목록 즉시 업데이트
       // UpdateCraftableItems();
    }
  
}

void CraftListPanelUI::RegisterUIObject(shared_ptr<GameObject> uiObject)
{
    // 필요시 추가 UI 오브젝트 등록
}
