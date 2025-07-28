#pragma once
#include "Player.h"
class Nicky :
    public Player
{
    using Super = Player;
public:
    Nicky(shared_ptr<Shader> _defaultShader);
    virtual ~Nicky();
public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void LateUpdate() override;
    virtual void FixedUpdate() override;

    //Collision ฐüทร
    virtual void OnCollision(shared_ptr<GameObject> _other) override;
    virtual void OnCollisionEnter(shared_ptr<GameObject> _other) override;
    virtual void OnCollisionExit(shared_ptr<GameObject> _other) override;


private:
    void InitNickyModel();
    void InitNickyAnimation();
    void InitNickyComponent();
    void InitNickySkill();
    void InitNickyStats();
public:
    virtual void Birth() override;
    virtual void Death() override;
    virtual void MakeItem() override;
    virtual void MakeFood() override;
};

