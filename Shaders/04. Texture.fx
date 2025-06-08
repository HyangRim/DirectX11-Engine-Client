matrix World;
matrix View;
matrix Projection;
Texture2D Texture0;

struct VertexInput
{
	float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    output.uv = input.uv;

	return output;
}

SamplerState Sampler0;

float4 PS(VertexOutput input) : SV_TARGET
{
    return Texture0.Sample(Sampler0, input.uv);
    //return input.color;
}

RasterizerState FillModeWireFrame
{
    FillMode = Wireframe;
};


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
