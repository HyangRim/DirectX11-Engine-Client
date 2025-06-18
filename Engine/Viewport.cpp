#include "pch.h"
#include "Viewport.h"

Viewport::Viewport()
{
	Set(1366, 768);
}

Viewport::Viewport(float _width, float _height, float _x, float _y, float _minDepth, float _maxDepth)
{
	Set(_width, _height, _x, _y, _minDepth, _maxDepth);
}

Viewport::~Viewport()
{
}

void Viewport::RSSetViewport()
{
	DC->RSSetViewports(1, &m_vp);
}

void Viewport::Set(float _width, float _height, float _x, float _y, float _minDepth, float _maxDepth)
{
	m_vp.TopLeftX = _x;
	m_vp.TopLeftY = _y;
	m_vp.Height = _height;
	m_vp.Width = _width;
	m_vp.MinDepth = _minDepth;
	m_vp.MaxDepth = _maxDepth;
}

Vec3 Viewport::Project(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P)
{
	Matrix wvp = W * V * P;


	//TransformCoord  -> ��ǥ�� ���ԵǾ� �̵� W�� 1
	//TransformNormal -> ���⸸ �̵� W�� 0

	Vec3 p = Vec3::Transform(pos, wvp);
	p.x = (p.x + 1.0f) * (m_vp.Width / 2) + m_vp.TopLeftX;
	p.y = (-p.y + 1.0f) * (m_vp.Height / 2) + m_vp.TopLeftY;
	p.z = p.z * (m_vp.MaxDepth - m_vp.MinDepth) + m_vp.MinDepth;

	return p;
}

Vec3 Viewport::UnProject(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P)
{
	Vec3 p = pos;

	p.x = 2.f * (p.x - m_vp.TopLeftX) / m_vp.Width - 1.f;
	p.y = -2.f * (p.y - m_vp.TopLeftY) / m_vp.Height + 1.f;
	p.z = ((p.z - m_vp.MinDepth) / (m_vp.MaxDepth - m_vp.MinDepth));

	Matrix wvp = W * V * P;
	Matrix wvpInv = wvp.Invert();

	p = Vec3::Transform(p, wvpInv);
	return p;
}
