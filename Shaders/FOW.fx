#include "00. Global.fx"
#include "00. Light.fx"
#include "00. Render.fx"

////////////////
// FOW Buffer //
////////////////

cbuffer FogOfWarData : register(b5)
{
    float3 g_playerWorldPos; // 플레이어 월드 위치
    float g_sightRange; // 시야 거리
    float g_darkness; // 어둠 강도 (0.0 ~ 1.0)
    float g_fadeDistance; // 페이드 거리
    float g_smoothness; // 경계 부드러움
    float g_time; // 시간 (애니메이션용)
}

////////////////
// Functions  //
////////////////

// FOW 계산 함수
float CalculateFogOfWar(float3 worldPos)
{
    // 플레이어로부터의 거리 계산
    float distance = length(worldPos - g_playerWorldPos);
    
    // 성능 최적화: 너무 멀리 있으면 조기 종료
    if (distance > g_sightRange * 1.2f)
    {
        return g_darkness;
    }
    
    float fogFactor = 1.0f;
    
    if (distance > g_sightRange)
    {
        // 시야 범위 밖: 어둡게
        fogFactor = g_darkness;
    }
    else if (distance > (g_sightRange - g_fadeDistance))
    {
        // 페이드 영역: 부드러운 전환
        float fadeRatio = (distance - (g_sightRange - g_fadeDistance)) / g_fadeDistance;
        
        // 부드러운 곡선 적용
        fadeRatio = smoothstep(0.0f, 1.0f, fadeRatio);
        fadeRatio = pow(fadeRatio, g_smoothness);
        
        fogFactor = lerp(1.0f, g_darkness, fadeRatio);
    }
    
    return fogFactor;
}

////////////////
// Pixel Shaders
////////////////

// 기본 FOW 픽셀 셰이더
float4 PS_FOW(MeshOutput input) : SV_TARGET
{
    // 기존 라이팅 계산
    float4 baseColor = ComputeLight(input.normal, input.uv, input.worldPosition);
    
    // FOW 계산
    float fogFactor = CalculateFogOfWar(input.worldPosition);
    
    // 최종 색상 계산
    float3 finalColor = baseColor.rgb * fogFactor;
    
    // 어둠 영역에 푸른빛 색조 추가 (분위기 연출)
    if (fogFactor < 0.5f)
    {
        float blueTint = (0.5f - fogFactor) * 0.15f;
        finalColor = lerp(finalColor, finalColor * float3(0.9f, 0.95f, 1.1f), blueTint);
    }
    
    return float4(finalColor, baseColor.a);
}

// 텍스처만 사용하는 간단한 FOW 셰이더
float4 PS_FOW_Simple(MeshOutput input) : SV_TARGET
{
    // 텍스처만 샘플링
    float4 baseColor = DiffuseMap.Sample(LinearSampler, input.uv);
    
    // FOW 계산
    float fogFactor = CalculateFogOfWar(input.worldPosition);
    
    // 최종 색상
    float3 finalColor = baseColor.rgb * fogFactor;
    
    return float4(finalColor, baseColor.a);
}

////////////////
// Techniques //
////////////////

// 메인 FOW 테크닉
technique11 T0
{
    // 기본 FOW 렌더링
    PASS_VP(P0, VS_Mesh, PS_FOW)
    PASS_VP(P1, VS_Model, PS_FOW)
    PASS_VP(P2, VS_Animation, PS_FOW)
    
    // 성능 최적화용 간단한 FOW
    PASS_VP(P3, VS_Mesh, PS_FOW_Simple)
    PASS_VP(P4, VS_Model, PS_FOW_Simple)
    PASS_VP(P5, VS_Animation, PS_FOW_Simple)
}

// 그림자 테크닉 (기존과 동일)
technique11 shadowTech
{
    PASS_SHADOW_V(P0, VS_Mesh)
    PASS_SHADOW_V(P1, VS_Model)
    PASS_SHADOW_V(P2, VS_Animation)
}