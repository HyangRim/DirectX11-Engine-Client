#include "pch.h"
#include "BiancaQProjectile.h"

BiancaQProjectile::BiancaQProjectile()
{
}

BiancaQProjectile::~BiancaQProjectile()
{
}

void BiancaQProjectile::Start()
{
}

void BiancaQProjectile::Update()
{
}

void BiancaQProjectile::OnCollision(shared_ptr<GameObject> _other)
{
}

void BiancaQProjectile::OnCollisionEnter(shared_ptr<GameObject> _other)
{
}

void BiancaQProjectile::OnCollisionExit(shared_ptr<GameObject> _other)
{
}

void BiancaQProjectile::SetMoveTarget(Vec3& _startPos, Vec3& _endPos)
{
	m_startPos = _startPos;
	m_endPos = _endPos;
	m_moving = true;
}
