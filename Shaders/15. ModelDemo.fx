#include "00. Global.fx"
#include "00. Light.fx"

#define MAX_MODEL_TRANSFORMS 50

cbuffer BoneBuffer
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
};

uint BoneIndex;

//00 Global���� �޾ƿ�. 
//pragma once�� �� ���� ������ �ǵ��� ��.

//matrix ToRootTransform
//�̷��� �ٷ� �־��༭ ������� ���� ������ ����?




MeshOutput VS(VertexTextureTangentNormal input)
{
    
    // W * V * P
    MeshOutput output;
    
    //�̰� �ϸ� ROOT�� ��ǥ��� �����. C -> ROOT�� 
    output.position = mul(input.position, BoneTransforms[BoneIndex]);
    
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    
    //��ü�� �� ȸ���� ���� normal�� ������ �� ����. 
    output.normal = mul(input.normal, (float3x3)W);
    
    output.tangent = mul(input.tangent, (float3x3) W);

	return output;
}

Texture2D Texture0;

//Ambient(�ֺ���/ȯ�汤)
//������ �ݻ縦 ���ļ� ������ �Һи��� ��
//������ ���� ������ ǥ��

//Diffuse(�л걤)
//��ü�� ǥ�鿡�� �л�Ǿ� ������ �ٷ� ������ ��
//������ ���� ��Ⱑ �ٸ���.

//Specular(�ݻ籤)
//�� �������� ������ �ݻ�Ǵ� ��(Phong)

//Emissive
//���� �ܰ��� ���� �� ���(������Ʈ)


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
	//���� ���� �н��� �д�. 
    PASS_VP(P0, VS, PS)
    PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)
};
