// InventoryManager.cpp
#include "pch.h"
#include "InventoryManager.h"
#include "Player.h"

InventoryManager::InventoryManager() : Super(ComponentType::Custom)
{
}

InventoryManager::~InventoryManager()
{
}

void InventoryManager::RegisterInventorySlots(const vector<shared_ptr<ItemSlot>>& inventorySlots)
{
    m_inventorySlots = inventorySlots;

    // 인벤토리 슬롯에 클릭 이벤트 등록
    for (int i = 0; i < m_inventorySlots.size(); i++) 
    {
        auto slot = m_inventorySlots[i];
        if (slot) 
        {
            // 슬롯 타입 설정
            slot->SetSlotType(SLOTTYPE::INVENTORY);

            // 클릭 이벤트 등록
            slot->OnSlotClicked.Push([this](int slotIndex, SLOTTYPE slotType) {
                OnInventorySlotClicked(slotIndex);
                });

            // 우클릭 이벤트도 필요하다면
            slot->OnSlotRightClicked.Push([this](int slotIndex, SLOTTYPE slotType) {
                // 우클릭 처리 로직
                //OnInventorySlotRightClicked(slotIndex);
                });
        }
    }
}

void InventoryManager::RegisterEquipmentSlots(const vector<shared_ptr<ItemSlot>>& equipmentSlots)
{
    m_equipmentSlots = equipmentSlots;

    // 장비 슬롯에 클릭 이벤트 등록
    for (int i = 0; i < m_equipmentSlots.size(); i++)
    {
        auto slot = m_equipmentSlots[i];
        if (slot)
        {
            // 슬롯 타입 설정
            slot->SetSlotType(SLOTTYPE::EQUIPMENT);

            // 클릭 이벤트 등록
            slot->OnSlotClicked.Push([this](int slotIndex, SLOTTYPE slotType) {
                OnEquipmentSlotClicked(slotIndex);
            });
        }
    }
}

bool InventoryManager::EquipItem(int inventorySlotIndex)
{
    if (inventorySlotIndex < 0 || inventorySlotIndex >= m_inventorySlots.size())
        return false;

    auto inventorySlot = m_inventorySlots[inventorySlotIndex];
    if (!inventorySlot || inventorySlot->IsEmpty())
        return false;

    auto item = inventorySlot->GetItem();
    if (item->GetItemType() != ITEMTYPE::EQUIPABLE)
        return false;

    // EquipItem으로 캐스팅
    auto equipItem = static_pointer_cast<EquipableItem>(item);
    if (!equipItem)
        return false;

    // 착용 가능한지 확인
    if (!CanEquipItem(equipItem))
        return false;

    // 해당 장비 타입의 슬롯 인덱스 찾기
    int equipSlotIndex = GetEquipmentSlotIndex(equipItem->GetEquipType());
    if (equipSlotIndex == -1)
        return false;

    auto equipmentSlot = m_equipmentSlots[equipSlotIndex];
    if (!equipmentSlot)
        return false;

    // 이미 장착된 아이템이 있다면 교체
    if (!equipmentSlot->IsEmpty()) {
        SwapItems(inventorySlot, equipmentSlot);
    }
    else {
        // 아이템 이동
        equipmentSlot->SetItem(equipItem);
        m_player->WearEquipment(equipItem);
        inventorySlot->ClearItem();
    }

    string tmp(equipItem->GetName().begin(), equipItem->GetName().end());
   // cout << "아이템 '" << tmp << "착용 완료!" << endl;
    return true;
}

bool InventoryManager::UnequipItem(int equipmentSlotIndex)
{
    if (equipmentSlotIndex < 0 || equipmentSlotIndex >= m_equipmentSlots.size())
        return false;

    auto equipmentSlot = m_equipmentSlots[equipmentSlotIndex];
    if (!equipmentSlot || equipmentSlot->IsEmpty())
        return false;

    // 인벤토리의 빈 슬롯 찾기
    int emptySlotIndex = -1;
    for (int i = 0; i < m_inventorySlots.size(); i++) {
        if (m_inventorySlots[i]->IsEmpty()) {
            emptySlotIndex = i;
            break;
        }
    }

    if (emptySlotIndex == -1) {
        //cout << "인벤토리에 빈 공간이 없습니다!" << endl;
        return false;
    }

    // 아이템 이동
    auto item = equipmentSlot->GetItem();
    m_player->TakeOffEquipment(equipmentSlot->GetSlotIndex());
    m_inventorySlots[emptySlotIndex]->SetItem(item);
    equipmentSlot->ClearItem();

    wstring name = item->GetName();
    string tmp(name.begin(), name.end());
    //cout << "아이템 '" << tmp << "' 해제 완료!" << endl;
    return true;
}

bool InventoryManager::MoveItem(int fromSlot, int toSlot, SLOTTYPE fromType, SLOTTYPE toType)
{
    shared_ptr<ItemSlot> sourceSlot, targetSlot;

    // 소스 슬롯 가져오기
    if (fromType == SLOTTYPE::INVENTORY) {
        if (fromSlot < 0 || fromSlot >= m_inventorySlots.size()) return false;
        sourceSlot = m_inventorySlots[fromSlot];
    }
    else {
        if (fromSlot < 0 || fromSlot >= m_equipmentSlots.size()) return false;
        sourceSlot = m_equipmentSlots[fromSlot];
    }

    // 타겟 슬롯 가져오기
    if (toType == SLOTTYPE::INVENTORY) {
        if (toSlot < 0 || toSlot >= m_inventorySlots.size()) return false;
        targetSlot = m_inventorySlots[toSlot];
    }
    else {
        if (toSlot < 0 || toSlot >= m_equipmentSlots.size()) return false;
        targetSlot = m_equipmentSlots[toSlot];
    }

    if (!sourceSlot || !targetSlot || sourceSlot->IsEmpty())
        return false;

    // 장비 슬롯으로 이동하는 경우 타입 검사
    if (toType == SLOTTYPE::EQUIPMENT) {
        auto item = sourceSlot->GetItem();
        if (item->GetItemType() != ITEMTYPE::EQUIPABLE) {
            //cout << "장비가 아닌 아이템은 장착할 수 없습니다!" << endl;
            return false;
        }

        auto equipItem = dynamic_pointer_cast<EquipableItem>(item);
        if (!equipItem) return false;

        // 올바른 장비 슬롯인지 확인
        if (GetEquipmentSlotIndex(equipItem->GetEquipType()) != toSlot) {
            //cout << "잘못된 장비 슬롯입니다!" << endl;
            return false;
        }
    }

    // 아이템 교체
    SwapItems(sourceSlot, targetSlot);
    return true;
}

int InventoryManager::GetEquipmentSlotIndex(EquipmentType equipType)
{
    // 장비 타입에 따른 슬롯 인덱스 반환
    switch (equipType) {
    case EquipmentType::WEAPON: return 0;  // 무기 슬롯
    case EquipmentType::CHEST:  return 1;  // 상의 슬롯
    case EquipmentType::HEAD:   return 2;  // 머리 슬롯
    case EquipmentType::ARM:    return 3;  // 팔 슬롯
    case EquipmentType::LEG:    return 4;  // 다리 슬롯
    default: return -1;
    }
}

void InventoryManager::OnInventorySlotClicked(int slotIndex)
{
   // cout << "인벤토리 슬롯 " << slotIndex << " 클릭됨!" << endl;

    if (slotIndex < 0 || slotIndex >= m_inventorySlots.size()) return;

    auto slot = m_inventorySlots[slotIndex];
    if (!slot) return;

    // 선택 상태 업데이트
    if (m_selectedSlot) {
        m_selectedSlot->SetSelected(false);
    }

    m_selectedSlot = slot;
    m_selectedSlotIndex = slotIndex;
    m_selectedSlotType = SLOTTYPE::INVENTORY;
    m_selectedSlot->SetSelected(true);

    auto item = slot->GetItem();
    if (item) {
        wstring name = item->GetName();
        string tmp(name.begin(), name.end());
       // cout << "선택된 아이템: " << tmp << endl;

        // 장비 아이템인 경우 자동 착용
        if (item->GetItemType() == ITEMTYPE::EQUIPABLE) {
            if (EquipItem(slotIndex)) {
                // 착용 성공시 선택 해제
                if (m_selectedSlot) {
                    m_selectedSlot->SetSelected(false);
                    m_selectedSlot = nullptr;
                    m_selectedSlotIndex = -1;
                }
            }
        }
    }
}

void InventoryManager::OnEquipmentSlotClicked(int slotIndex)
{
    //cout << "장비 슬롯 " << slotIndex << " 클릭됨!" << endl;

    if (slotIndex < 0 || slotIndex >= m_equipmentSlots.size()) return;

    auto slot = m_equipmentSlots[slotIndex];
    if (!slot) return;

    // 선택 상태 업데이트
    if (m_selectedSlot) {
        m_selectedSlot->SetSelected(false);
    }

    m_selectedSlot = slot;
    m_selectedSlotIndex = slotIndex;
    m_selectedSlotType = SLOTTYPE::EQUIPMENT;
    m_selectedSlot->SetSelected(true);

    auto item = slot->GetItem();
    if (item) {

        wstring name = item->GetName();
        string tmp(name.begin(), name.end());
        //cout << "선택된 장비: " << tmp << endl;

        // 장비 해제
        UnequipItem(slotIndex);
       
        // 해제 후 선택 해제
        if (m_selectedSlot) {
            m_selectedSlot->SetSelected(false);
            m_selectedSlot = nullptr;
            m_selectedSlotIndex = -1;
        }
    }
}

bool InventoryManager::CanEquipItem(shared_ptr<EquipableItem> equipItem)
{
    if (!equipItem) return false;


    // 추가적인 착용 조건 검사 (레벨, 클래스 등)
    // 현재는 기본적으로 true 반환
    return true;
}

void InventoryManager::SwapItems(shared_ptr<ItemSlot> slot1, shared_ptr<ItemSlot> slot2)
{
    auto item1 = slot1->GetItem();
    auto item2 = slot2->GetItem();


    m_player->WearEquipment(static_pointer_cast<EquipableItem>(item1));


    slot1->SetItem(item2);
    slot2->SetItem(item1);
}

void InventoryManager::Update()
{
    Super::Update();

    // 추가적인 업데이트 로직이 필요하면 여기에 구현
}
