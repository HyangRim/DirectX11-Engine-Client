#include "00. Global.fx"

//00 Global에서 받아옴. 
//pragma once는 한 번만 컴파일 되도록 함. 

MeshOutput VS(VertexTextureNormal input)
{
    
    // W * V * P
    MeshOutput output;
    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    
    //물체의 그 회전에 따라 normal이 딸라질 수 있음. 
    output.normal = mul(input.normal, (float3x3)W);

	return output;
}

Texture2D Texture0;

float4 PS(MeshOutput input) : SV_TARGET
{
   
    return Texture0.Sample(LinearSampler, input.uv);
    //return input.color;
}



//Filter = 확대/축소 일어났을 때 중간값을 처리하는 방식
//Address = UV가 1보다 컸을 때, 나머지 부분을 어떻게 처리할 것인지. 


//technique11 <- 기본적인 문법. 
//pass도 기본적인 문법. 

//이걸 이용해서 어떤 애를 사용할 지 골라줄 수 있음. 
technique11 T0
{
	//여러 개의 패스를 둔다. 
    PASS_VP(P0, VS, PS)
};
