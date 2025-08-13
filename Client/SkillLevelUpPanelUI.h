#pragma once
#include "IBasePanelUI.h"

class Player;

class SkillLevelUpPanelUI :
    public IBasePanelUI
{
public:
    SkillLevelUpPanelUI(shared_ptr<Player> player);
    virtual ~SkillLevelUpPanelUI();

    virtual void Initialize();
    virtual void Update();
    virtual void SetVisible(bool visible);
    virtual void Cleanup();


    void UpdateSkillLevelPanel();

protected:
    virtual void LoadResources();
    virtual void CreatePanels();

    virtual void RegisterUIObject(shared_ptr<GameObject> uiObject);

private:
    shared_ptr<Player> m_player;
};

