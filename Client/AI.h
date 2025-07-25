#pragma once

class Monster;
class AI
{
public:
	AI(shared_ptr<Monster> _Owner);
	virtual ~AI();
public:
	virtual void Update() = 0;

private:
	friend class Monster;

	shared_ptr<Monster> m_Owner;
};

