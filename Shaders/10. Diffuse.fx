#include "00. Global.fx"

//00 Global���� �޾ƿ�. 
//pragma once�� �� ���� ������ �ǵ��� ��.

float3 LightDir;
float4 LightDiffuse;
float4 MaterialDiffuse;
Texture2D DiffuseMap;

float4 LightAmbient;
float4 MaterialAmbient;



MeshOutput VS(VertexTextureNormal input)
{
    
    // W * V * P
    MeshOutput output;
    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    
    //��ü�� �� ȸ���� ���� normal�� ������ �� ����. 
    output.normal = mul(input.normal, (float3x3)W);

	return output;
}

Texture2D Texture0;

//Ambient(�ֺ���/ȯ�汤)
//������ �ݻ縦 ���ļ� ������ �Һи��� ��
//������ ���� ������ ǥ��

//Diffuse(�л걤)
//��ü�� ǥ�鿡�� �л�Ǿ� ������ �ٷ� ������ ��
//������ ���� ��Ⱑ �ٸ���.

float4 PS(MeshOutput input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    
    //������ ������� ��Į���� ���´�. 
    float value = dot(-LightDir, normalize(input.normal));
    color = color * value * LightDiffuse * MaterialDiffuse;
    //return Texture0.Sample(LinearSampler, input.uv) * color;
    return color;
}



technique11 T0
{
	//���� ���� �н��� �д�. 
    PASS_VP(P0, VS, PS)
};
