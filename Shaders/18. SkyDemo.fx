#include "00. Global.fx"
#include "00. Light.fx"


//00 Global에서 받아옴. 
//pragma once는 한 번만 컴파일 되도록 함.

//matrix ToRootTransform
//이렇게 바로 넣어줘서 계산하지 않은 이유가 뭔가?


//CBuffer은 많은 데이터를 넣을 수 있는 게 아니다.


struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};



VS_OUT VS(VertexTextureTangentNormal input)
{
    
    // W * V * P
    VS_OUT output;
    
    //Local -> World -> View -> Proj
    //물체가 카메라의 위치랑 똑같아야 한다.
    
    //TODO(Blend)
    //float4 worldPos = mul(input.position, W);
    //world 행렬 건너뛰고, View로 하면.그냥 원점에서 움직이는거랑 똑같으니까. 
    //카메라를 두리번 두리번하면, 어색해지는 거 방지. 
    float4 viewPos = mul(float4(input.position.xyz, 0), V);
    
    float4 clipPos = mul(viewPos, P);
    output.position = clipPos.xyzw;
    output.position.z = output.position.w * 0.999999f;
    //이렇게 넣어주면, 깊이값(Z)는 어떻게 되는가> 
    
    //xyzw
    //Raserizer -> w보존. 
    //양 옆에 w를 넣어줬기 떄문에 깊이값이 무조건 1이 뜬다. 
    
    output.uv = input.uv;
    
    //xy[-1 - 1], z[0 - 1]범위. 

	return output;
}

Texture2D Texture0;

//Ambient(주변광/환경광)
//수많은 반사를 거쳐서 광원이 불분명한 빛
//일정한 밝기와 색으로 표현

//Diffuse(분산광)
//물체의 표면에서 분산되어 눈으로 바로 들어오는 빛
//각도에 따라 밝기가 다르다.

//Specular(반사광)
//한 방향으로 완전히 반사되는 빛(Phong)

//Emissive
//보통 외곽선 구할 때 사용(림라이트)


float4 PS(VS_OUT input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    return color;
}



technique11 T0
{
	//여러 개의 패스를 둔다. 
    //PASS_VP(P0, VS, PS)
    //PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)

    pass P0
    {
        SetRasterizerState(FrontCounterClickwiseTrue);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
};
