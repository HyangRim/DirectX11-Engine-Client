#include "00. Global.fx"

//00 Global에서 받아옴. 
//pragma once는 한 번만 컴파일 되도록 함.

float3 LightDir;
float4 LightSpecular;
float4 MaterialSpecular;

Texture2D DiffuseMap;



MeshOutput VS(VertexTextureNormal input)
{
    
    // W * V * P
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = input.position;
    
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

//Diffuse(분산광)
//물체의 표면에서 분산되어 눈으로 바로 들어오는 빛
//각도에 따라 밝기가 다르다.

//Specular(반사광)
//한 방향으로 완전히 반사되는 빛(Phong)

float4 PS(MeshOutput input) : SV_TARGET
{
    float3 R = reflect(LightDir, input.normal);
    R = normalize(R);
    float3 cameraPosition = -V._41_42_43;
    float3 E = normalize(cameraPosition - input.worldPosition);
    
    float value = saturate(dot(R, E)); // clamp(0 ~ 1)
    float specular = pow(value, 10);
    
    float4 color = LightSpecular * MaterialSpecular * specular;
    
    return color;
}



technique11 T0
{
	//여러 개의 패스를 둔다. 
    PASS_VP(P0, VS, PS)
};
