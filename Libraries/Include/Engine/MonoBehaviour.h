#pragma once
#include "Component.h"
class MonoBehaviour :
    public Component
{
    using Super = Component;

public:
    MonoBehaviour();
    virtual ~MonoBehaviour();

    virtual void Init() override;
    virtual void Update() override;
};

