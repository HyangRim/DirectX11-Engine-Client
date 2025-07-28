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
    float4 screenPos : TEXCOORD1; // 스크린 좌표 추가
};

// 버텍스 셰이더
VertexOutput2 VS(VertexInput2 input)
{
    VertexOutput2 output;
    
    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);
    output.screenPos = output.position; // 스크린 좌표 저장
    output.uv = input.uv;
    
    return output;
}

// 클리핑이 적용된 픽셀 셰이더
float4 PS_Clipped(VertexOutput2 input) : SV_TARGET
{
    // 스크린 좌표로 변환
    float2 screenPos = input.screenPos.xy / input.screenPos.w;
    screenPos = screenPos * 0.5f + 0.5f; // [-1,1] -> [0,1]
    screenPos.y = 1.0f - screenPos.y; // Y축 뒤집기
    
    // 뷰포트 크기로 스케일링
    screenPos.x *= 1366.0f; // 실제 화면 너비로 조정
    screenPos.y *= 768.0f; // 실제 화면 높이로 조정
    
    // 클리핑 영역 체크
    if (EnableClipping)
    {
        if (screenPos.x < ClippingRect.x || screenPos.x > ClippingRect.z ||
            screenPos.y < ClippingRect.y || screenPos.y > ClippingRect.w)
        {
            discard; // 클리핑 영역 밖의 픽셀은 버림
        }
    }
    
    float4 color = DiffuseMap.Sample(ImageSampler, input.uv);
    return color;
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

// 픽셀 셰이더 (단색 사용 - Material 색상 사용)
float4 PS_SolidColor(VertexOutput2 input) : SV_TARGET
{
    return Material.diffuse; // Material 상수 버퍼의 diffuse 색상 사용
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

    // 단색 알파 블렌딩 패스
    pass P2
    {
        SetDepthStencilState(UIDepthStencil, 0);
        SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_SolidColor()));
    }

    // 단색 패스
    pass P3
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_SolidColor()));
    }

    // 클리핑 패스 추가
    pass P4
    {
        SetDepthStencilState(UIDepthStencil, 0);
        SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Clipped()));
    }
}

#endif
