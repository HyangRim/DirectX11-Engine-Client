#pragma once
#include "Component.h"
class Light : public Component
{

public:
	Light();
	virtual ~Light();

	virtual void Update();


public:
	LightDesc& GetLightDesc() { return m_desc; }

	void SetLightDesc(LightDesc& desc) { m_desc = desc; }
	void SetAmbient(const Color& color) { m_desc.ambient = color; }
	void SetDiffuse(const Color& color) { m_desc.diffuse = color; }
	void SetSpecular(const Color& color) { m_desc.specular = color; }
	void SetEmissive(const Color& color) { m_desc.emissive = color; }
	void SetLightDirection(Vec3 direction) { m_desc.direction = direction; }

private:
	LightDesc m_desc;
};

