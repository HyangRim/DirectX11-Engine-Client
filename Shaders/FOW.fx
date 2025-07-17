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
        return max(g_darkness, 0.4f);
    }
    
    float fogFactor = 1.0f;
    
    if (distance > g_sightRange)
    {
        // 시야 범위 밖: 어둡게
        fogFactor = max(g_darkness, 0.4f);
    }
    else if (distance > (g_sightRange - g_fadeDistance))
    {
        // 페이드 영역: 부드러운 전환
        float fadeRatio = (distance - (g_sightRange - g_fadeDistance)) / g_fadeDistance;
        
        // 부드러운 곡선 적용
        fadeRatio = smoothstep(0.0f, 1.0f, fadeRatio);
        fadeRatio = pow(fadeRatio, g_smoothness);
        
        fogFactor = lerp(1.0f, max(g_darkness, 0.4f), fadeRatio);
    }
    
    return fogFactor;
}

////////////////
// Pixel Shaders
////////////////

// 기본 FOW 픽셀 셰이더
float4 PS_FOW(MeshOutput input) : SV_TARGET
{
    //그림자 계산. 
    float shadow = CalcShadowFactor(ShadowMap, input.shadowPosH);
    // 기존 라이팅 계산
    float4 baseColor = ComputeLight(input.normal, input.uv, input.worldPosition, shadow);
    
    // FOW 계산
    float fogFactor = CalculateFogOfWar(input.worldPosition);
    
    // 회색 필터 효과 계산
    float3 grayColor = dot(baseColor.rgb, float3(0.299, 0.587, 0.114)); // 그레이스케일 변환
    grayColor = grayColor * float3(0.7, 0.7, 0.8); // 약간 푸른빛이 도는 회색
    
    float grayIntensity = saturate(g_smoothness * 0.5f);
    
    // 원본 색상과 회색 사이의 보간 (어두운 영역일수록 회색 강함)
    float3 foggedColor = lerp(baseColor.rgb, grayColor, (1.0f - fogFactor) * grayIntensity);
    
    // 최소 밝기 보장 (g_darkness를 최소 밝기로 재해석)
    float minBrightness = max(g_darkness, 0.4f);
    
    // 최종 색상 계산
    float3 finalColor = foggedColor * max(fogFactor, minBrightness);
    
    // 어둠 영역에 푸른빛 색조 추가 (분위기 연출)
    if (fogFactor < 0.6f)
    {
        float blueTint = (0.6f - fogFactor) * 0.1f;
        finalColor = lerp(finalColor, finalColor * float3(0.9f, 0.95f, 1.05f), blueTint);
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

float4 PS_FOW_DEBUG(MeshOutput input) : SV_TARGET
{
    return float4(1, 0, 0, 1); // 간단한 형태
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