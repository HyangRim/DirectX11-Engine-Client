#include "00. Global.fx"
#include "00. Light.fx"
#include "00. Render.fx"


struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float2 scale : SCALE;
};


struct V_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 shadowPosH : TEXCOORD2;
};


V_OUT VS(VertexInput input)
{
    V_OUT output;
    
    float4 position = mul(input.position, W);
    float3 up = float3(0, 1, 0);
   // float3 forward = float3(0, 0, 1);
    float3 forward = position.xyz - CameraPosition();
    float3 right = normalize(cross(up, forward));
    
    position.xyz += (input.uv.x - 0.5f) * right * input.scale.x;
    position.xyz += (1.0f - input.uv.y - 0.5f) * up * input.scale.y;
    position.w = 1.0f;
    
    output.shadowPosH = mul(position, ShadowTransform);
    
    output.position = mul(mul(position, V), P);
    
    output.uv = input.uv;
    
    return output;
}

float4 PS(V_OUT input) : SV_Target{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.uv);
    
    
     
    if (diffuse.a < 0.5f)
        discard;
    return diffuse;

}

float4 ShadowedPS(V_OUT input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.uv);
    
    if (diffuse.a < 0.5f)
        discard;
    
    float shadow = CalcShadowFactor(ShadowMap, input.shadowPosH);
    diffuse = diffuse * saturate(GlobalLight.ambient.r + shadow);
    return diffuse;

}

technique11 T0
{
   
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, ShadowedPS()));
    }

};
technique11 ShadowTech
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
        SetRasterizerState(DepthNoCull);
    }
};