#pragma once
#include "GameObject.h"


class Item;
class AI;

struct MonsterStatus {
    int level = 1;
    int32 maxHp = 200;
    int32 hp = 200;

    float adPower = 50;
    float hitRange = 2.5;
    float hitSpeed = 0.3f;

    float moveSpeed = 1.56;
};

class Monster :
    public GameObject
{
    using Super = GameObject;
public:
    Monster();
    virtual ~Monster();

public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void LateUpdate() override;
    virtual void FixedUpdate() override;

    //Collision ฐüทร
    virtual void OnCollision(shared_ptr<GameObject> _other) = 0;
    virtual void OnCollisionEnter(shared_ptr<GameObject> _other) = 0;
    virtual void OnCollisionExit(shared_ptr<GameObject> _other) = 0;

public:
    MonsterStatus& GetMonsterStatus() { return m_monsterStatus; }

    void SetLevel(int _value) { m_monsterStatus.level = _value; }
    void SetMaxHP(int32 _value) { m_monsterStatus.maxHp = _value; }
    void SetHP(int32 _value) { m_monsterStatus.hp = _value; }
    void SetAD(float _value) { m_monsterStatus.adPower = _value; }
    void SetHitRange(float _value) { m_monsterStatus.hitRange = _value; }
    void SetHitSpeed(float _value) { m_monsterStatus.hitSpeed = _value; }
    void SetMoveSpeed(float _value) { m_monsterStatus.moveSpeed = _value; }

private:
    MonsterStatus m_monsterStatus;
    shared_ptr<AI> m_ai;
};

