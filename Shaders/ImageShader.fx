#ifndef _IMAGE_SHADER_FX_
#define _IMAGE_SHADER_FX_

#include "00. Global.fx"
#include "00. Light.fx"
#include "00. Render.fx"


SamplerState ImageSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

// 버텍스 셰이더 입력
struct VertexInput2
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

// 버텍스 셰이더 출력
struct VertexOutput2
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

// 버텍스 셰이더
VertexOutput2 VS(VertexInput2 input)
{
    VertexOutput2 output;
    
    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    
    return output;
}

// 픽셀 셰이더
float4 PS(VertexOutput2 input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(ImageSampler, input.uv);
    
    //// 알파 테스트 (완전히 투명한 픽셀은 버림)
    //if (color.a < 0.01)
    //    discard;
    
    return color;
}

// 알파 블렌딩을 위한 픽셀 셰이더
float4 PS_Alpha(VertexOutput2 input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(ImageSampler, input.uv);
    
    // // 알파 테스트 (완전히 투명한 픽셀은 버림)
    //if (color.a < 1)
    //    discard;
    
    return color;
}

// 테크닉
technique11 T0
{
    // 기본 패스 (알파 테스트)
    pass P0
    {
 
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
    
    // 알파 블렌딩 패스
    pass P1
    {
        SetDepthStencilState(UIDepthStencil, 0); // UI 전용 깊이 스텐실 상태
        SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

#endif
