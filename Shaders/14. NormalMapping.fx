#include "00. Global.fx"
#include "00. Light.fx"

//00 Global���� �޾ƿ�. 
//pragma once�� �� ���� ������ �ǵ��� ��.




MeshOutput VS(VertexTextureTangentNormal input)
{
    
    // W * V * P
    MeshOutput output;
    output.position = mul(input.position, W);
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
    ComputeNormalMapping(input.normal, input.tangent, input.uv);
    
    float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);
    
    return color;
}



technique11 T0
{
	//���� ���� �н��� �д�. 
    PASS_VP(P0, VS, PS)
};
