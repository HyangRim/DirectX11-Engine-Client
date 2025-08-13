#include "pch.h"
#include "UIManager.h"

#include "GameHUDPanelUI.h"
#include "SkillLevelUpPanelUI.h"
#include "PlayerStatusPanelUI.h"
#include "TimePanelUI.h"
#include "DayPanelUI.h"
#include "EquipmentPanelUI.h"
#include "InventoryPanelUI.h"

UIManager::UIManager(shared_ptr<Player> player, int selectedCharIdx)
	:Super(ComponentType::Custom)
	, m_player(player)
{
	m_gameHUD = make_shared<GameHUDPanelUI>(player, selectedCharIdx);
	m_skillLevelUp = make_shared<SkillLevelUpPanelUI>(player);
	m_playerStatus = make_shared<PlayerStatusPanelUI>(player);
	m_time = make_shared<TimePanelUI>();
	m_day = make_shared<DayPanelUI>();
	m_equipment = make_shared<EquipmentPanelUI>();
	m_inventory = make_shared<InventoryPanelUI>();
}

UIManager::~UIManager()
{

}

void UIManager::Update()
{
	m_gameHUD->Update();
	m_skillLevelUp->Update();
	m_playerStatus->Update();
	m_time->Update();
	m_day->Update();
	m_equipment->Update();
	m_inventory->Update();
}

void UIManager::InitializeUI()
{
	m_gameHUD->Initialize();
	m_skillLevelUp->Initialize();
	m_playerStatus->Initialize();
	m_time->Initialize();
	m_day->Initialize();

	m_equipment->Initialize();
	m_inventory->Initialize();
	CreateInventoryManager();
}

void UIManager::UpdateUI()
{

}

void UIManager::CreateInventoryManager()
{
	auto managerObj = make_shared<GameObject>();
	managerObj->SetName(L"InventoryManager");

	m_inventoryManager = make_shared<InventoryManager>();
	managerObj->AddComponent(m_inventoryManager);

	// ½½·Ôµé µî·Ï
	m_inventoryManager->RegisterInventorySlots(m_inventory->GetInventroySlots());
	m_inventoryManager->RegisterEquipmentSlots(m_equipment->GetEquipmentSlots());
	m_inventoryManager->SetPlayer(m_player);
}