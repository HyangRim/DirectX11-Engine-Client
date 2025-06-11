#include "00. Global.fx"
#include "00. Light.fx"

#define MAX_MODEL_TRANSFORMS 50

cbuffer BoneBuffer
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
};

uint BoneIndex;

//00 Global에서 받아옴. 
//pragma once는 한 번만 컴파일 되도록 함.

//matrix ToRootTransform
//이렇게 바로 넣어줘서 계산하지 않은 이유가 뭔가?




MeshOutput VS(VertexTextureTangentNormal input)
{
    
    // W * V * P
    MeshOutput output;
    
    //이걸 하면 ROOT의 좌표계로 변경됨. C -> ROOT로 
    output.position = mul(input.position, BoneTransforms[BoneIndex]);
    
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    
    //물체의 그 회전에 따라 normal이 딸라질 수 있음. 
    output.normal = mul(input.normal, (float3x3)W);
    
    output.tangent = mul(input.tangent, (float3x3) W);

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


float4 PS(MeshOutput input) : SV_TARGET
{
    //ComputeNormalMapping(input.normal, input.tangent, input.uv);
    
    //float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);
    
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    return color;
}

float4 PS_RED(MeshOutput input) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}

technique11 T0
{
	//여러 개의 패스를 둔다. 
    PASS_VP(P0, VS, PS)
    PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)
};
