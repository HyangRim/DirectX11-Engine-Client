#pragma once
#include "Renderer.h"

class Mesh;
class Shader;
class Material;

#define MAX_MESH_INSTANCE 500

class MeshRenderer : public Renderer
{
	using Super = Renderer;
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	//virtual void Update() override;

	void SetMesh(shared_ptr<Mesh> _mesh) { m_mesh = _mesh; }
	void SetMaterial(shared_ptr<Material> _material) { m_material = _material; }
	void SetPass(uint8 _pass) { m_pass = _pass; }

	void RenderInstancing(shared_ptr<class InstancingBuffer>& _buffer);
	InstanceID GetInstanceID();

private:
	shared_ptr<Mesh> m_mesh;
	//shared_ptr<Texture> m_texture;
	//shared_ptr<Shader> m_shader;
	//�ؽ�ó�� ���̴��� ���׸��� ������ ��. 
	shared_ptr<Material> m_material;

	uint8 m_pass = 0;
};

