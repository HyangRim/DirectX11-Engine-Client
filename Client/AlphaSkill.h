#pragma once

class BiancaESkillCircle;
class Alpha;
class AlphaSkill
{
public:
	AlphaSkill(shared_ptr<Alpha> _alpha);
	~AlphaSkill();

public:
	void Start();
	void Play();
	void Update();

private:
	shared_ptr<BiancaESkillCircle> m_circleObjects[5];
	shared_ptr<Alpha> m_alpha;

	float m_skillElapsedTime = 0.f;
	float m_skillDuration = 2.25f;

	float m_skillEffectDuration = 1.f;
	float m_skillEffectElapsedTime = 0.f;

	float m_skilRange = 6.5f;
	bool m_isActive = false;
	bool m_boozer = false;
};

