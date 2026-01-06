// 셰이더 파일에 디버그 심볼 추가
#pragma enable_d3d11_debug_symbols

#include "00. Global.fx"
#include "00. Light.fx"
#include "00. Render.fx"
#include "00. GBuffer.fx"
#include "00. DeferredLighting.fx"



////////////////
// Functions  //
////////////////


////////////////
// Pixel Shaders
////////////////

// 기본 FOW 픽셀 셰이더
float4 PS_FOW(MeshOutput input) : SV_TARGET
{
    
    //float distance = length(input.worldPosition - g_playerWorldPos);
    //if (distance > 25.f)
    //{
    //    discard;
    //}
    
    
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

float4 PS_FOW_Transparency(MeshOutput input) : SV_TARGET
{
    return float4(0, 0, 0, 0.5);
}

// NavMesh 전용 픽셀 셰이더 추가
float4 PS_NavMesh_Debug(MeshOutput input) : SV_TARGET
{
    return float4(0.0f, 1.0f, 0.0f, 0.5f); // 반투명 녹색
}

float4 PS_NavMesh_Wireframe(MeshOutput input) : SV_TARGET
{
    return float4(0.0f, 0.8f, 0.0f, 1.0f); // 불투명 녹색
}

// G-Buffer 출력용 픽셀 셰이더
GBufferOutput PS_GBuffer(MeshOutput input)
{
    GBufferOutput output;
    
    // 디퓨즈 텍스처 샘플링
    float4 albedo = DiffuseMap.Sample(LinearSampler, input.uv);
    
    if (albedo.a < 0.05f)
        discard;
    
    // G-Buffer 데이터 출력
    output.albedo = albedo;
    //output.albedo = float4(0, 0, 0, 1);
    //return output;
    output.normal = float4(normalize(input.normal), 1.0f);
    output.position = float4(input.worldPosition, input.position.z);
    output.material = float4(1, 1, 1, 1); // 기본 머티리얼 속성
    
    // Material.r에 ObjectType 저장
    float objectTypeValue = 0.0f;
    if (objType == 1)
    { // ITEMBOX
        objectTypeValue = 1.0f;
    }
    
    output.material = float4(objectTypeValue, 0.0f, 0.0f, 1.0f);
    
    
    
    // 임시 디버깅: 단계적으로 테스트
    //output.albedo = float4(1, 0, 0, 1); // 빨간색으로 고정
    //output.normal = float4(0, 1, 0, 1); // 초록색으로 고정  
    //output.position = float4(0, 0, 1, 1); // 파란색으로 고정
    //output.material = float4(1, 1, 0, 1); // 노란색으로 고정
    return output;
}


float4 PS_DebugSRV(VertexQuadOutput IN) : SV_Target
{
    // t0만 샘플링
    float4 a = GBufferAlbedo.Sample(LinearSampler, IN.uv);
    
    if (a.r == 0 && a.g == 0 && a.b == 0 && a.a == 0)
    {
        return float4(1, 0, 1, 1);
    }
    return a;
}

// 4분할 디버그 뷰 픽셀 셰이더
float4 PS_DebugGBuffer2(VertexQuadOutput input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 finalColor = float4(0, 0, 0, 1);

    // 4분할 영역 판별
    // 좌상단 (0,0 ~ 0.5, 0.5) -> Albedo
    if (uv.x < 0.5 && uv.y < 0.5)
    {
        float2 localUV = uv * 2.0f; // 0~0.5 범위를 0~1로 확장
        finalColor = GBufferAlbedo.Sample(LinearSampler, localUV);
    }
    // 우상단 (0.5,0 ~ 1.0, 0.5) -> Normal
    else if (uv.x >= 0.5 && uv.y < 0.5)
    {
        float2 localUV = float2((uv.x - 0.5f) * 2.0f, uv.y * 2.0f);
        float4 normal = GBufferNormal.Sample(LinearSampler, localUV);
        // Normal은 -1~1 범위일 수 있으므로 보기 좋게 0~1로 변환
        finalColor = float4((normal.xyz + 1.0f) * 0.5f, 1.0f);
    }
    // 좌하단 (0, 0.5 ~ 0.5, 1.0) -> Position
    else if (uv.x < 0.5 && uv.y >= 0.5)
    {
        float2 localUV = float2(uv.x * 2.0f, (uv.y - 0.5f) * 2.0f);
        float4 pos = GBufferPosition.Sample(LinearSampler, localUV);
        // Position 값은 범위가 커서 그냥 출력하면 하얗게만 보일 수 있음.
        // 시각화를 위해 적당히 나누거나 frac 등을 사용하기도 함. 여기선 그대로 출력.
        // finalColor = float4(frac(pos.xyz), 1.0f); // 패턴으로 보고 싶을 때
        finalColor = float4(pos.xyz / 100.0f, 1.0f); // 스케일 조정 (예시)
    }
    // 우하단 (0.5, 0.5 ~ 1.0, 1.0) -> Material
    else
    {
        float2 localUV = float2((uv.x - 0.5f) * 2.0f, (uv.y - 0.5f) * 2.0f);
        finalColor = GBufferMaterial.Sample(LinearSampler, localUV);
    }

    return finalColor;
}


float4 PS_DeferredLightingWithFOW2(VertexQuadOutput input) : SV_TARGET
{
    // 1. G-Buffer 샘플링
    float4 albedo = GBufferAlbedo.Sample(LinearSampler, input.uv);
    float3 position = GBufferPosition.Sample(LinearSampler, input.uv).xyz;
    float3 normal = GBufferNormal.Sample(LinearSampler, input.uv).xyz;

    // 초기 색상 (Ambient)
    float3 finalColor = float3(0, 0, 0);

    // 2. 다중 광원 루프
    for (int i = 0; i < g_activeLightCount; ++i)
    {
        if (g_lights[i].lightType == 1) // Point Light
        {
            float dist = distance(g_lights[i].position, position);
            if (dist < g_lights[i].range)
            {
                // 감쇠 (Attenuation)
                float att = 1.0f - (dist / g_lights[i].range);
                att = att * att; // 제곱 감쇠로 자연스럽게

                // Diffuse (Lambert)
                float3 lightDir = normalize(g_lights[i].position - position);
                float diffuse = max(dot(normal, lightDir), 0.0f);

                // 결과 누적
                finalColor += albedo.rgb * g_lights[i].diffuse.rgb * diffuse * att;
            }
        }
        else if (g_lights[i].lightType == 0) // Directional Light
        {
            float3 lightDir = normalize(-g_lights[i].direction);
            float diffuse = max(dot(normal, lightDir), 0.0f);
            finalColor += albedo.rgb * g_lights[i].diffuse.rgb * diffuse;
        }
    }
    
    // FOW 적용 등 후처리...
    return float4(finalColor, 1.0f);
 
}

////////////////
// Techniques //
////////////////

// 메인 FOW 테크닉
technique11 T0
{
    // 기본 FOW 렌더링
    //PASS_VP(P0, VS_Mesh, PS_FOW)
    //PASS_VP(P1, VS_Model, PS_FOW)

    //PASS_VP(P2, VS_Animation, PS_FOW)

    PASS_VP(P0, VS_Mesh, PS_GBuffer)
    //PASS_VP(P0, VS_Quad, PS_DebugGBuffer2)


    PASS_VP(P1, VS_Model, PS_GBuffer)
    PASS_VP(P2, VS_Animation, PS_GBuffer)

    PASS_RS_VP(P3, FillModeWireFrame, VS_Mesh, PS_FOW_DEBUG)
    PASS_VP(P4, VS_Mesh, PS_FOW_Transparency)
// NavMesh 디버그 렌더링 추가
    PASS_BS_VP(P6, AlphaBlend, VS_Mesh, PS_NavMesh_Debug)
    PASS_RS_VP(P7, FillModeWireFrame, VS_Mesh, PS_NavMesh_Wireframe)

    PASS_VP(P8, VS_Quad, PS_DebugGBuffer2)
    
}

// 그림자 테크닉 (기존과 동일)
technique11 shadowTech
{
    PASS_SHADOW_V(P0, VS_Mesh)
    PASS_SHADOW_V(P1, VS_Model)
    PASS_SHADOW_V(P2, VS_Animation)
}

technique11 DeferredLightingTech
{
    PASS_VP(P0, VS_Quad, PS_DeferredLightingWithFOW)
}

technique11 GBufferTech
{
    PASS_VP(P0, VS_Mesh, PS_GBuffer)
    PASS_VP(P1, VS_Model, PS_GBuffer)
    PASS_VP(P2, VS_Animation, PS_GBuffer)
    PASS_RS_VP(P3, FillModeWireFrame, VS_Mesh, PS_FOW_DEBUG)
    PASS_VP(P4, VS_Mesh, PS_FOW_Transparency)
}
