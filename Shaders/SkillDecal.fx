#include "00. Global.fx"
#include "00. Light.fx"

// 데칼 전용 버퍼만 새로 추가 (기존 아웃라인 패턴과 동일)
cbuffer DecalBuffer
{
    matrix DecalMatrix;
    matrix InvDecalMatrix;
    float4 DecalColor;
    float DecalAlpha;
    float3 DecalPadding;
};

// 데칼 텍스처들
Texture2D DecalTexture;
Texture2D DepthTexture;

// 기존 MeshOutput 구조체 재활용
MeshOutput VS_Decal(VertexTextureNormalTangent input)
{
    MeshOutput output;
    
    // 기존 월드 변환 방식 재활용 (W 행렬 사용)
    float4 worldPos = mul(input.position, W);
    output.position = mul(worldPos, VP);
    output.worldPosition = worldPos.xyz;
    output.uv = input.uv;
    output.normal = input.normal;
    output.tangent = input.tangent;
    
    // shadowPosH는 데칼에서 사용하지 않지만 구조체 호환성을 위해 설정
    output.shadowPosH = float4(0, 0, 0, 0);
    
    return output;
}


// 더 간단한 버전
float4 PS_Decal(MeshOutput input) : SV_Target
{
    return float4(0, 1, 0, 1);
    // 현재 데칼 위치를 데칼 로컬 공간으로 변환
    float3 decalPos = mul(float4(input.worldPosition, 1.0), InvDecalMatrix).xyz;
    
    // 데칼 박스 경계 체크
    clip(0.5 - abs(decalPos.xyz));
    
    // UV 좌표 계산
    float2 decalUV = decalPos.xz + 0.5;
    
    // 텍스처 샘플링
    float4 decalColor = DecalTexture.Sample(LinearSampler, decalUV);
    decalColor *= DecalColor;
    decalColor.a *= DecalAlpha;
    
    return decalColor;
}

// 기존 technique 명명 패턴 재활용
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Decal()));
        SetPixelShader(CompileShader(ps_5_0, PS_Decal()));
    }
}
