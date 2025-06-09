#pragma once
#include "Component.h"

class Mesh;
class Shader;

class MeshRenderer : public Component
{
	using Super = Component;
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	virtual void Update() override;

	void SetMesh(shared_ptr<Mesh> mesh) { m_mesh = mesh; }
	void SetTexture(shared_ptr<Texture> texture) { m_texture = texture; }
	void SetShader(shared_ptr<Shader> shader) { m_shader = shader; }

private:
	shared_ptr<Mesh> m_mesh;
	shared_ptr<Texture> m_texture;
	shared_ptr<Shader> m_shader;
};

