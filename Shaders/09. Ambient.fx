#include "00. Global.fx"

//00 Global에서 받아옴. 
//pragma once는 한 번만 컴파일 되도록 함. 

float4 LightAmbient;
float4 MaterialAmbient;


MeshOutput VS(VertexTextureNormal input)
{
    
    // W * V * P
    MeshOutput output;
    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    
    //물체의 그 회전에 따라 normal이 딸라질 수 있음. 
    output.normal = mul(input.normal, (float3x3)W);

	return output;
}

Texture2D Texture0;

//Ambient(주변광/환경광)
//수많은 반사를 거쳐서 광원이 불분명한 빛
//일정한 밝기와 색으로 표현

float4 PS(MeshOutput input) : SV_TARGET
{
    float4 color = LightAmbient * MaterialAmbient;
    
    return Texture0.Sample(LinearSampler, input.uv) * color;
    //return color;
}



technique11 T0
{
	//여러 개의 패스를 둔다. 
    PASS_VP(P0, VS, PS)
};
