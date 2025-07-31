#pragma once
#include "GameObject.h"

class Item;
class EquipItem;
class BaseSkill;
struct ItemStatus;

struct PlayerStatus {
    int level = 1;
    int curExpLimit = 50;
    int curExp = 0;

    int32 max_HP = 500;
    int32 hp = 500;
    int32 max_Stamina = 300;
    int32 stamina = 300;
    
    float hitAttack = 50;
    float hitRange = 15;
    float hitSpeed = 0.54f;
    float defense = 30;
    float cooldownReduction = 0.f;
    float moveSpeed = 3.14;

    //0.25초마다 회복되는 양. 
    float healing = 7.2;
    float healing_Stamina = 11.4;
};


struct PlayerGrowStatus {
    int ExpLimit = 25;

    int32 hp = 78;
    int32 stamina = 46;

    float HitAttack = 5;
    float hitSpeed = 0.04f;
    float defense = 7;

    float healing = 0.56;
    float healing_Stamina = 0.62;
};
class Player :
    public GameObject
{
    using Super = GameObject;
public:
    Player();
    virtual ~Player();
public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void LateUpdate() override;
    virtual void FixedUpdate() override;

    //Collision 관련
    virtual void OnCollision(shared_ptr<GameObject> _other) = 0;
    virtual void OnCollisionEnter(shared_ptr<GameObject> _other) = 0;
    virtual void OnCollisionExit(shared_ptr<GameObject> _other) = 0;

public:
    void WearEquipment(shared_ptr<EquipItem> _item);
    void TakeOffEquipment(int _index);

    void LevelUp();
    virtual void Birth() = 0;
    virtual void Death() = 0;
    virtual void MakeItem() = 0;
    virtual void MakeFood() = 0;

public:
    //Helper Function.
    PlayerStatus& GetStatus() { return m_status; }
    shared_ptr<Shader> GetShader() { return m_defaultShader; }
    
    void SetLevel(int _value) { m_status.level = _value; }
    void SetCurExpLimit(int _value) { m_status.curExpLimit = _value; }
    void SetCurExp(int _value) { m_status.curExp = _value; LevelUp(); }
    void SetMaxHP(int32 _value) { m_status.max_HP = _value; }
    void SetHP(int32 _value) {
        if (_value > m_status.max_HP)
            m_status.hp = m_status.max_HP;
        else
            m_status.hp = _value;
    }
    void SetMaxStamina(int32 _value) { m_status.max_Stamina = _value; }
    void SetStamina(int32 _value) {
        if (_value > m_status.max_Stamina)
            m_status.stamina = m_status.max_Stamina;
        else
            m_status.stamina = _value;
    }

    void SetHitAttack(float _value) { m_status.hitAttack = _value; }
    void SetHitRange(float _value) { m_status.hitRange = _value; }
    void SetHitSpeed(float _value) { m_status.hitSpeed = _value; }
    void SetDefense(float _value) { m_status.defense = _value; }
    void SetCooldownReduction(float _value) { m_status.cooldownReduction = _value; }
    void SetMoveSpeed(float _value) { m_status.moveSpeed = _value; }
    void SetHealing(float _value) { m_status.healing = _value; }
    void SetHealingStamina(float _value) { m_status.healing_Stamina = _value; }


private:
    void ApplyEquipStatus(ItemStatus& _Equipstatus);
    void ReleaseEquipStatus(ItemStatus& _Equipstatus);
    

protected:
    PlayerStatus m_status;
    PlayerGrowStatus m_growStatus;

    //Q,W,E,R 스킬.
    array<shared_ptr<BaseSkill>, 4> m_skills;

    //무기, 상의, 머리, 팔, 다리 순서. 
    array<shared_ptr<EquipItem>, 5> m_curEquipment;

    //모든 아이템 전부 보유 가능.(기타, 소비, 장비)
    array<shared_ptr<Item>, 10> m_inventory;

    //모델, 애니메이터
    shared_ptr<Model> m_model;
    shared_ptr<Rigidbody> m_rigidbody;
    shared_ptr<SphereCollider> m_collider;
    shared_ptr<NavMeshAgent> m_navAgent;
    shared_ptr<Shader> m_defaultShader;


    //그 이외에 UI들(체력, 경험치 등등) 
    //연동 위해서 필요함. (따로 UI클래스들 만들어야함)
    

private:
    float m_healingCoolTime = 0.f;
};

