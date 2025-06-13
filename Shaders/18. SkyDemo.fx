#include "00. Global.fx"
#include "00. Light.fx"


//00 Global���� �޾ƿ�. 
//pragma once�� �� ���� ������ �ǵ��� ��.

//matrix ToRootTransform
//�̷��� �ٷ� �־��༭ ������� ���� ������ ����?


//CBuffer�� ���� �����͸� ���� �� �ִ� �� �ƴϴ�.


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
    //��ü�� ī�޶��� ��ġ�� �Ȱ��ƾ� �Ѵ�.
    
    //TODO(Blend)
    //float4 worldPos = mul(input.position, W);
    //world ��� �ǳʶٰ�, View�� �ϸ�.�׳� �������� �����̴°Ŷ� �Ȱ����ϱ�. 
    //ī�޶� �θ��� �θ����ϸ�, ��������� �� ����. 
    float4 viewPos = mul(float4(input.position.xyz, 0), V);
    
    float4 clipPos = mul(viewPos, P);
    output.position = clipPos.xyzw;
    output.position.z = output.position.w * 0.999999f;
    //�̷��� �־��ָ�, ���̰�(Z)�� ��� �Ǵ°�> 
    
    //xyzw
    //Raserizer -> w����. 
    //�� ���� w�� �־���� ������ ���̰��� ������ 1�� ���. 
    
    output.uv = input.uv;
    
    //xy[-1 - 1], z[0 - 1]����. 

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


float4 PS(VS_OUT input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    return color;
}



technique11 T0
{
	//���� ���� �н��� �д�. 
    //PASS_VP(P0, VS, PS)
    //PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)

    pass P0
    {
        SetRasterizerState(FrontCounterClickwiseTrue);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
};
