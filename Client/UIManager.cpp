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
#include "CraftGagePanelUI.h"

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


	//craftList에서 craftGage를 제어하는 부분이 있음 순서 중요
	m_craftGage = make_shared<CraftGagePanelUI>(player);
	m_craftList = make_shared<CraftListPanelUI>(player, m_craftGage);
	
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
	m_craftGage->Update();
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
	m_craftGage->Initialize();
}

void UIManager::UpdateUI()
{

}

void UIManager::CreateInventoryManager()
{

	// 슬롯들 등록
	InventoryManager::GetInstance()->RegisterInventorySlots(m_inventory->GetInventorySlots());
	InventoryManager::GetInstance()->RegisterEquipmentSlots(m_equipment->GetEquipmentSlots());
	InventoryManager::GetInstance()->SetPlayer(m_player);
}