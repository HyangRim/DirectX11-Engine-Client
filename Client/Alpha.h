#pragma once
#include "Monster.h"
class Alpha :
    public Monster
{
    using Super = Monster;
public:
    Alpha(shared_ptr<Shader> _shader);
    virtual ~Alpha();

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
    void UpdateState();

private:
    void InitAlphaModel();
    void InitAlphaAnimation();
    void InitAlphaComponent();
    void InitAlphaAI();
    void InitAlphaStats();
};