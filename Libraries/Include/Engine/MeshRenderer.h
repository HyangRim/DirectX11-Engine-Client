#pragma once
#include "Component.h"

class Mesh;
class Shader;
class Material;

class MeshRenderer : public Component
{
	using Super = Component;
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	//virtual void Update() override;

	void SetMesh(shared_ptr<Mesh> _mesh) { m_mesh = _mesh; }
	void SetMaterial(shared_ptr<Material> _material) { m_material = _material; }

	void SetTexture(shared_ptr<Texture> _texture) {  }
	void SetShader(shared_ptr<Shader> _shader) {  }
	void SetPass(uint8 _pass) { m_pass = _pass; }

	void RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer);
	InstanceID GetInstanceID();

private:
	shared_ptr<Mesh> m_mesh;
	//shared_ptr<Texture> m_texture;
	//shared_ptr<Shader> m_shader;
	//텍스처랑 셰이더는 머테리얼 안으로 들어감. 
	shared_ptr<Material> m_material;

	uint8 m_pass = 0;
};

