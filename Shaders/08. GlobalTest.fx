#include "00. Global.fx"

//00 Global���� �޾ƿ�. 
//pragma once�� �� ���� ������ �ǵ��� ��. 

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

float4 PS(MeshOutput input) : SV_TARGET
{
   
    return Texture0.Sample(LinearSampler, input.uv);
    //return input.color;
}



//Filter = Ȯ��/��� �Ͼ�� �� �߰����� ó���ϴ� ���
//Address = UV�� 1���� ���� ��, ������ �κ��� ��� ó���� ������. 


//technique11 <- �⺻���� ����. 
//pass�� �⺻���� ����. 

//�̰� �̿��ؼ� � �ָ� ����� �� ����� �� ����. 
technique11 T0
{
	//���� ���� �н��� �д�. 
    PASS_VP(P0, VS, PS)
};
