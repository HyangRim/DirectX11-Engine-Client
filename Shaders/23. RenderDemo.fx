#include "00. Global.fx"
#include "00. Light.fx"
#include "00. Render.fx"


float4 PS(MeshOutput input) : SV_TARGET
{
    //float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);
    float shadow = CalcShadowFactor(ShadowMap, input.shadowPosH);
    float4 color = ComputeLight(input.normal, input.uv, input.worldPosition, shadow);
    return color;
}


//GPU�� CPU�� �� �� �� ���� ��Ű�� ��. 
//GPU�� ���� ����, �ΰ����ɰ� ��ȣȭ�󿡼� ComputeShader�� ��.

//GPU���� � ������ ���� �й��� ���ΰ�??
technique11 T0
{
	//���� ���� �н��� �д�. 
    PASS_VP(P0, VS_Mesh, PS)
    PASS_VP(P1, VS_Model, PS)
    PASS_VP(P2, VS_Animation, PS)
    //PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)

};

technique11 shadowTech
{
	PASS_SHADOW_V(P0, VS_Mesh)
	PASS_SHADOW_V(P1, VS_Model)
	PASS_SHADOW_V(P2, VS_Animation)
};
