#pragma once
#include "Monster.h"
class Wolf :
    public Monster
{
    using Super = Monster;
public:
    Wolf();
    virtual ~Wolf();

public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void LateUpdate() override;
    virtual void FixedUpdate() override;

    //Collision ฐüทร
    virtual void OnCollision(shared_ptr<GameObject> _other) = 0;
    virtual void OnCollisionEnter(shared_ptr<GameObject> _other) = 0;
    virtual void OnCollisionExit(shared_ptr<GameObject> _other) = 0;

private:
    void UpdateState();

private:
    void InitWolfModel();
    void InitWolfAnimation();
    void InitWolfComponent();
    void InitWolfAI();
    void InitWolfStats();
};

