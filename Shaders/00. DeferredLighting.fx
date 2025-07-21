#ifndef _DLIGHT_FX_
#define _DLIGHT_FX_


#include "00. Global.fx"
#include "00. Light.fx"

Texture2D GBufferAlbedo : register(t0);
Texture2D GBufferNormal : register(t1);
Texture2D GBufferPosition : register(t2);
Texture2D GBufferMaterial : register(t3);

// FOW Buffer
cbuffer FogOfWarData : register(b5)
{
    float3 g_playerWorldPos;
    float g_sightRange;
    float g_darkness;
    float g_fadeDistance;
    float g_smoothness;
    float g_time;
}

//여러 개 라이트 지원
#define MAX_LIGHTS 20
cbuffer MultiLightBuffer
{
    LightDesc lIGHTS[MAX_LIGHTS];
    int ActiveLightCount;
    float3 Mpadding;
};

struct VertexQuad
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexQuadOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VertexQuadOutput VS_Quad(VertexQuad input)
{
    VertexQuadOutput output;
    output.position = input.position;
    output.uv = input.uv;
    return output;
}

// FOW 계산 함수 (기존과 동일)
float CalculateFogOfWar(float3 worldPos)
{
    float distance = length(worldPos - g_playerWorldPos);
    
    if (distance > g_sightRange * 1.2f)
    {
        return max(g_darkness, 0.4f);
    }
    
    float fogFactor = 1.0f;
    
    if (distance > g_sightRange)
    {
        fogFactor = max(g_darkness, 0.4f);
    }
    else if (distance > (g_sightRange - g_fadeDistance))
    {
        float fadeRatio = (distance - (g_sightRange - g_fadeDistance)) / g_fadeDistance;
        fadeRatio = smoothstep(0.0f, 1.0f, fadeRatio);
        fadeRatio = pow(fadeRatio, g_smoothness);
        
        fogFactor = lerp(1.0f, max(g_darkness, 0.4f), fadeRatio);
    }
    
    return fogFactor;
}

float4 PS_DeferredLightingWithFOW(VertexQuadOutput input) : SV_Target
{
    int2 screenPos = (int2) (input.position.xy);
    
    float4 albedo = GBufferAlbedo.Load(int3(screenPos, 0));
    float4 normalData = GBufferNormal.Load(int3(screenPos, 0));
    float4 positionData = GBufferPosition.Load(int3(screenPos, 0));
    
    if (albedo.a < 0.01f)
        discard;
    
    float3 worldPos = positionData.xyz;
    float3 normal = normalData.xyz;
    
    //그림자 계산
    float4 shadowPosH = mul(float4(worldPos, 1.0f), ShadowTransform);
    float shadow = CalcShadowFactor(ShadowMap, shadowPosH);
    
    float4 baseColor = ComputeLight(normal, input.uv, worldPos, shadow);
    
    float fogFactor = CalculateFogOfWar(worldPos);
    
    // FOW 효과 적용
    float3 grayColor = dot(baseColor.rgb, float3(0.299, 0.587, 0.114));
    grayColor = grayColor * float3(0.7, 0.7, 0.8);
    
    float grayIntensity = saturate(g_smoothness * 0.5f);
    float3 foggedColor = lerp(baseColor.rgb, grayColor, (1.0f - fogFactor) * grayIntensity);
    
    float minBrightness = max(g_darkness, 0.4f);
    float3 finalColor = foggedColor * max(fogFactor, minBrightness);
    
    if (fogFactor < 0.6f)
    {
        float blueTint = (0.6f - fogFactor) * 0.1f;
        finalColor = lerp(finalColor, finalColor * float3(0.9f, 0.95f, 1.05f), blueTint);
    }
    
    return float4(finalColor, baseColor.a);
}

technique11 DeferredTech
{
    PASS_VP(P0, VS_Quad, PS_DeferredLightingWithFOW)
}
#endif