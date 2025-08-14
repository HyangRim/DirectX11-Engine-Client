#include "pch.h"
#include "InventoryPanelUI.h"

#include "EquipableItem.h"
#include "Item.h"
#include "ItemSlot.h"
#include "ItemManager.h"

InventoryPanelUI::InventoryPanelUI()
{

}

InventoryPanelUI::~InventoryPanelUI()
{

}

void InventoryPanelUI::Initialize()
{
	//LoadResources();
	CreatePanels();
}

void InventoryPanelUI::Update()
{

}

void InventoryPanelUI::SetVisible(bool visible)
{

}

void InventoryPanelUI::Cleanup()
{

}

void InventoryPanelUI::LoadResources()
{

}

void InventoryPanelUI::CreatePanels()
{
	m_panel = make_shared<GameObject>();
	m_panel->SetName(L"CharMainPanel");

	auto panel = make_shared<UIPanel>();
	m_panel->AddComponent(panel);

	panel->Create(Vec2(960.f, 768 - 57), Vec2(252, 62), Vec4(0.f, 0.f, 0.f, 0.f), nullptr);
	m_panel->SetLayerIndex(LAYER_UI);

	CURSCENE->AddUIObject(m_panel, true);
	CURSCENE->RegisterUIParent(m_panel);

	CreateInventorySlots();
}

void InventoryPanelUI::RegisterUIObject(shared_ptr<GameObject> uiObject)
{

}

void InventoryPanelUI::CreateInventorySlots()
{
	m_inventorySlots.clear();

	// 5x2 그리드로 10개 슬롯 생성
	int slotsX = 5;
	int slotsY = 2;
	Vec2 slotSize = Vec2(46, 28);
	Vec2 spacing = Vec2(5, 5);
	Vec2 startPos = Vec2(960.f - (252 / 2.f) + 23, (768 - 57) - (62 / 2.f) + 14); // 패널 내 시작 위치
	Vec2 panelSize = Vec2(252, 62);

	for (int row = 0; row < slotsY; row++)
	{
		for (int col = 0; col < slotsX; col++)
		{
			int slotIndex = row * slotsX + col;

			// ItemSlot 생성
			shared_ptr<ItemSlot> itemSlot;

			if		(row == 0 && col == 0) itemSlot = make_shared<ItemSlot>(ItemManager::GetInstance()->GetItem(L"운명의 수레바퀴"), true);
			else if (row == 0 && col == 1) itemSlot = make_shared<ItemSlot>(ItemManager::GetInstance()->GetItem(L"어사의"), true);
			else if (row == 0 && col == 2) itemSlot = make_shared<ItemSlot>(ItemManager::GetInstance()->GetItem(L"비질란테"), true);
			else if (row == 0 && col == 3) itemSlot = make_shared<ItemSlot>(ItemManager::GetInstance()->GetItem(L"나이팅게일"), true);
			else if (row == 0 && col == 4) itemSlot = make_shared<ItemSlot>(ItemManager::GetInstance()->GetItem(L"델타 레드"), true);

			else itemSlot =	make_shared<ItemSlot>(ItemManager::GetInstance()->GetItem(L"돌멩이"), true);

			itemSlot->SetSlotType(SLOTTYPE::INVENTORY);
			Vec2 slotPos = Vec2(
				startPos.x + col * (slotSize.x + spacing.x),
				startPos.y + row * (slotSize.y + spacing.y)
			);
			itemSlot->CreateSlot(slotPos, slotSize, slotIndex);

			m_inventorySlots.push_back(itemSlot);
		}
	}
}
