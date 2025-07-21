#ifndef _GBUFFER_FX_
#define _GBUFFER_FX_


#include "00. Global.fx"
#include "00. Light.fx"
#include "00. Render.fx"

struct GBufferOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 position : SV_Target2;
    float4 material : SV_Target3;
};

GBufferOutput PS_GeometryPass(MeshOutput input)
{
    GBufferOutput output;
    float4 albedo = DiffuseMap.Sample(LinearSampler, input.uv);
    
    if (albedo.a < 0.01f)
        discard;
    
    output.albedo = albedo;
    output.normal = float4(normalize(input.normal), 1.0f);
    output.position = float4(input.worldPosition, input.position.z);
    output.material = float4(1, 1, 1, 1);
    
    return output;
}

technique11 GeometryTech
{
    PASS_VP(P0, VS_Mesh, PS_GeometryPass)
    PASS_VP(P1, VS_Model, PS_GeometryPass)
    PASS_VP(P2, VS_Animation, PS_GeometryPass)
}
#endif