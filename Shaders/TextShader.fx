#ifndef _TEXT_FX_
#define _TEXT_FX_

#include "00. Global.fx"

////////////////
// Structures //
////////////////

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
    // Instancing
    uint instanceID : SV_INSTANCEID;
    matrix world : INST;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

////////////////
// Resources  //
////////////////

Texture2D DiffuseMap;

cbuffer TextMaterialBuffer
{
    float4 TextColor; // 내부 글자 색상
    float4 OutlineColor; // 외곽선 색상
    float TextAlpha;
    float OutlineWidth; // 외곽선 두께
    float2 TextPadding;
};

////////////////
// Vertex Shader //
////////////////

PixelInput VS_Text(VertexInput input)
{
    PixelInput output;
    
    // World 변환
    float4 worldPos = mul(input.position, input.world);
    
    // View-Projection 변환
    output.position = mul(worldPos, VP);
    
    // UV 좌표 전달
    output.uv = input.uv;
    
    // 텍스트 색상 전달
    output.color = TextColor;
    output.color.a *= TextAlpha;
    
    return output;
}

////////////////
// Pixel Shader //
////////////////

float4 PS_OutlineText(PixelInput input) : SV_TARGET
{
    float2 uv = input.uv;
    
    // 텍스처에서 원본 색상 샘플링
    float4 texColor = DiffuseMap.Sample(LinearSampler, uv);
    
    // 텍셀 크기 계산 (외곽선 두께에 따라 조정)
    float2 texelSize = OutlineWidth / float2(512.0f, 512.0f); // 텍스처 크기 가정
    
    // 8방향 외곽선 샘플링
    float outline = 0.0f;
    outline += DiffuseMap.Sample(LinearSampler, uv + float2(-texelSize.x, -texelSize.y)).a;
    outline += DiffuseMap.Sample(LinearSampler, uv + float2(0.0f, -texelSize.y)).a;
    outline += DiffuseMap.Sample(LinearSampler, uv + float2(texelSize.x, -texelSize.y)).a;
    outline += DiffuseMap.Sample(LinearSampler, uv + float2(-texelSize.x, 0.0f)).a;
    outline += DiffuseMap.Sample(LinearSampler, uv + float2(texelSize.x, 0.0f)).a;
    outline += DiffuseMap.Sample(LinearSampler, uv + float2(-texelSize.x, texelSize.y)).a;
    outline += DiffuseMap.Sample(LinearSampler, uv + float2(0.0f, texelSize.y)).a;
    outline += DiffuseMap.Sample(LinearSampler, uv + float2(texelSize.x, texelSize.y)).a;
    outline /= 8.0f;
    
    // 최종 색상 결정
    float4 finalColor;
    
    if (texColor.a > 0.5f)
    {
        // 텍스트 내부: 지정된 텍스트 색상 사용
        finalColor = input.color;
        finalColor.a *= texColor.a;
    }
    else if (outline > 0.1f)
    {
        // 외곽선 영역: 외곽선 색상 사용
        finalColor = OutlineColor;
        finalColor.a *= outline * TextAlpha;
    }
    else
    {
       
        discard;
    }
    
    return finalColor;
}

////////////////
// Technique //
////////////////

technique11 TextTech
{
    // Pass 0: 외곽선 포함 텍스트
    pass P0
    {
        SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFF);
        SetDepthStencilState(NoDepthWrites, 0);
        SetVertexShader(CompileShader(vs_5_0, VS_Text()));
        SetPixelShader(CompileShader(ps_5_0, PS_OutlineText()));
    }
}

#endif
