#pragma once
#include "GameObject.h"
class BiancaESkillCircle :
    public GameObject
{
    using Super = GameObject;

public:
    BiancaESkillCircle();
    virtual ~BiancaESkillCircle();

    virtual void Start() override;
    virtual void Update() override;
    virtual void LateUpdate() override;

    //Collision ฐüทร
    virtual void OnCollision(shared_ptr<GameObject> _other);

public:
    void DamageFlag(bool _value) { m_damageFlag = _value; }
    unordered_set<shared_ptr<GameObject>> GetCollisionObjects() { return m_object; }
private:
    bool m_damageFlag = false;
    unordered_set<shared_ptr<GameObject>> m_object;
};

