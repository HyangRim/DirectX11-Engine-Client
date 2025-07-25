#pragma once
#include "Item.h"

enum class EquipmentType {
    WEAPON,
    CLOTHES,
    HAT,
    ARM,
    LEGS
};

struct ItemStatus {
    int32 hp = 0;
    int32 stamina = 0;
    float ad = 0;
    float ap = 0;

    //기본 공속에 더하기 방식 
    float hitSpeed = 0;
    float defense = 0;
    float cooldownReduction = 0;
    float moveSpeed = 0;
    float healing = 0;
    float healing_Stamina = 0;
};

class EquipItem :
    public Item
{
    using super = Item;
public:
    EquipItem();
    virtual ~EquipItem();

public:
    void SetStatus(ItemStatus& _status) { m_status = _status; }
    ItemStatus& GetStatus() { return m_status; }
    
    void SetEquipType(EquipmentType _type) { _type = m_EquipType; }
    EquipmentType GetEquipType() { return m_EquipType; }

private:

    //Equip에서 Item 변동 값은 
    ItemStatus m_status;
    EquipmentType m_EquipType;
};

