#include "00. Global.fx"
#include "00. Light.fx"


//00 Global에서 받아옴. 
//pragma once는 한 번만 컴파일 되도록 함.

//matrix ToRootTransform
//이렇게 바로 넣어줘서 계산하지 않은 이유가 뭔가?


//CBuffer은 많은 데이터를 넣을 수 있는 게 아니다.



//수많은 인스턴싱에서 World 좌표만은 제각기 다르기에, 
//그것만 어떻게든 넣어줄 수 있으면 된다. 



#define MAX_MODEL_TRANSFORMS 250

cbuffer BoneBuffer
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
};

uint BoneIndex;

struct VS_IN
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 blendIndices : BLEND_INDICES;
    float4 blendWeights : BLEND_WEIGHTS;
    
    //INSTANCING
    uint instanceID : SV_INSTANCEID;
    matrix world : INST;
};


struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};



VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    
    //Model Global로 해서, 모델 내부의 메쉬들 Global좌표로 변경하기. 
    output.position = mul(input.position, BoneTransforms[BoneIndex]);
    
    output.position = mul(output.position, input.world); // W 
    output.worldPosition = output.position;
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.normal = input.normal;
    
    return output;
}

float4 PS(VS_OUT input) : SV_TARGET
{
    //float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    return color;
}



technique11 T0
{
	//여러 개의 패스를 둔다. 
    PASS_VP(P0, VS, PS)
    //PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)

};
