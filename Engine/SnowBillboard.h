#pragma once
#include "Component.h"

struct VertexSnow {
    Vec3 pos;
    Vec2 uv;
    Vec2 scale;
    Vec2 random;
};

#define MAX_BILLBOARD_COUNT 500

class SnowBillboard :
    public Component
{
    using Super = Component;

public:
    SnowBillboard(Vec3 _extent, int32 _drawCount = 100);
    virtual ~SnowBillboard();

    void Update();

    void SetMaterial(shared_ptr<Material> _material) { m_material = _material; }
    void SetPass(uint8 _pass) { m_pass = _pass; }

private:
    vector<VertexSnow> m_vertices;
    vector<uint32> m_indices;
    shared_ptr<VertexBuffer> m_vertexBuffer;
    shared_ptr<IndexBuffer> m_indexBuffer;

    int32 m_drawCount = 0;

    shared_ptr<Material> m_material;
    uint8 m_pass = 0;

    SnowBillboardDesc m_desc;
    float m_elapsedTime = 0.f;
};

