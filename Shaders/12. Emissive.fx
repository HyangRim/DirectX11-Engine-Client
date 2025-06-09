#include "00. Global.fx"

//00 Global���� �޾ƿ�. 
//pragma once�� �� ���� ������ �ǵ��� ��.

float4 MaterialEmissive;

Texture2D DiffuseMap;



MeshOutput VS(VertexTextureNormal input)
{
    
    // W * V * P
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = input.position;
    
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

//Specular(�ݻ籤)
//�� �������� ������ �ݻ�Ǵ� ��(Phong)

//Emissive
//���� �ܰ��� ���� �� ���(������Ʈ)


float4 PS(MeshOutput input) : SV_TARGET
{
    float3 cameraPosition = -V._41_42_43;
    float3 E = normalize(cameraPosition - input.worldPosition);
    
    float value = saturate(dot(E, input.normal));
    float emissive = 1.0f - value;
    
    //min, max, x (smooth lerp����)
    emissive = smoothstep(0.0f, 1.0f, emissive);
    emissive = pow(emissive, 3);
    float4 color = MaterialEmissive * emissive;
    
    
    
    return color;
}



technique11 T0
{
	//���� ���� �н��� �д�. 
    PASS_VP(P0, VS, PS)
};
