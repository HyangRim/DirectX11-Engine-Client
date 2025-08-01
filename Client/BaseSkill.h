#pragma once

class Player;
class BaseSkill
{
public:
	BaseSkill(shared_ptr<Player> _player);
	virtual ~BaseSkill();

public:
	virtual void Update();
	virtual void PlaySkill();
	void SkillEnd();

public:
	void SetSkillName(wstring& _name) { m_skillName = _name; }
	void SetSkillName(wstring&& _name) { m_skillName = move(_name); }
	wstring& GetSkillName() { return m_skillName; }

	void SetSkillDesc(wstring& _desc) { m_skillDesc = _desc; }
	void SetSkillDesc(wstring&& _desc) { m_skillDesc = move(_desc); }
	wstring& GetSkillDesc() { return m_skillDesc; }

	void SetSkillAnimsName(const vector<wstring>& _animsName) {m_skillAnimsName = _animsName;}
	vector<wstring> GetSkillAnimsName() { return m_skillAnimsName; }

	void SetSkillImage(shared_ptr<Texture> _image) { m_skillImage = _image; }
	shared_ptr<Texture> GetSkillImage() { return m_skillImage; }

	float GetCurCooldown() { return m_skillcurCooldown; }
	
	XMVECTOR ScreenToWorld(POINT _screenPos);

protected:
	bool m_isPassive = false;
	wstring m_skillName;
	wstring m_skillDesc;
	
	int m_skillLevel;
	//스킬 사용 시 이 값으로 초기화. 
	float m_skillCooldown;
	//현재 스킬 쿨다운, 
	float m_skillcurCooldown;

	vector<wstring> m_skillAnimsName;
	shared_ptr<Texture> m_skillImage;

	shared_ptr<Player> m_playerObject;
};

