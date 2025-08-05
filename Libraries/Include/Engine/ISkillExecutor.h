#pragma once
class ISkillExecutor
{
public:
    virtual ~ISkillExecutor() = default;

    // 스킬 실행 관련
    virtual bool CanExecuteSkill() const = 0;
    virtual void ExecuteSkill() = 0;

    // 쿨다운 관련
    virtual float GetCurrentCooldown() const = 0;
    virtual float GetMaxCooldown() const = 0;
    virtual bool IsOnCooldown() const = 0;
    virtual void StartCooldown() = 0;

    // 업데이트
    virtual void UpdateCooldown(float deltaTime) = 0;

    // 스킬 정보
    virtual int GetSkillIndex() const = 0;
    virtual const wstring& GetSkillName() const = 0;


};

