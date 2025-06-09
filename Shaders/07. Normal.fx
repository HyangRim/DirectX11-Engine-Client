matrix World;
matrix View;
matrix Projection;
Texture2D Texture0;
float3 LightDir;

uint Address;



struct VertexInput
{
	float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    output.uv = input.uv;
    
    //물체의 그 회전에 따라 normal이 딸라질 수 있음. 
    output.normal = mul(input.normal, (float3x3) World);

	return output;
}

SamplerState Sampler0
{
    AddressU = Wrap;
    AddressV = Wrap;
};





float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 light = -LightDir;
    
    //return float4(1, 1, 1, 1) * dot(light, normal);
    //world좌표 기반으로 빛 노말 연산. 
    return Texture0.Sample(Sampler0, input.uv) * dot(light, normal);
    //return input.color;
}

RasterizerState FillModeWireFrame
{
    FillMode = Wireframe;
};


//Filter = 확대/축소 일어났을 때 중간값을 처리하는 방식
//Address = UV가 1보다 컸을 때, 나머지 부분을 어떻게 처리할 것인지. 


//technique11 <- 기본적인 문법. 
//pass도 기본적인 문법. 

//이걸 이용해서 어떤 애를 사용할 지 골라줄 수 있음. 
technique11 T0
{
	//여러 개의 패스를 둔다. 
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}

    pass P1
    {
        SetRasterizerState(FillModeWireFrame);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
};
