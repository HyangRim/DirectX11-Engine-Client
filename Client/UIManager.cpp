#include "pch.h"
#include "UIManager.h"

#include "GameHUDPanelUI.h"
#include "SkillLevelUpPanelUI.h"
#include "PlayerStatusPanelUI.h"
#include "TimePanelUI.h"
#include "DayPanelUI.h"
#include "EquipmentPanelUI.h"
#include "InventoryPanelUI.h"
#include "CraftListPanelUI.h"

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
	m_craftList = make_shared<CraftListPanelUI>(player);
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
	m_craftList->Update();
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

	m_craftList->Initialize();
}

void UIManager::UpdateUI()
{

}

void UIManager::CreateInventoryManager()
{

	// ½½·Ôµé µî·Ï
	InventoryManager::GetInstance()->RegisterInventorySlots(m_inventory->GetInventroySlots());
	InventoryManager::GetInstance()->RegisterEquipmentSlots(m_equipment->GetEquipmentSlots());
	InventoryManager::GetInstance()->SetPlayer(m_player);
}