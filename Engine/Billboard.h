#pragma once
#include "Component.h"

struct VertexBillboard {
    Vec3 pos;
    Vec2 uv;
    Vec2 scale;
};

#define MAX_BILLBOARD_COUNT 500

class Billboard :
    public Component
{
    using Super = Component;

public:
    Billboard();
    virtual ~Billboard();

    void Update();
    void Add(Vec3 _pos, Vec2 _scale);

    void SetMaterial(shared_ptr<Material> _material) { m_material = _material; }
    void SetPass(uint8 _pass) { m_pass = _pass; }

private:
    vector<VertexBillboard> m_vertices;
    vector<uint32> m_indices;
    shared_ptr<VertexBuffer> m_vertexBuffer;
    shared_ptr<IndexBuffer> m_indexBuffer;

    int32 m_drawCount = 0;
    int32 m_prevCount = 0;

    shared_ptr<Material> m_material;
    uint8 m_pass = 0;
};

