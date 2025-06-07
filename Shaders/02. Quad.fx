
struct VertexInput
{
	float4 position : POSITION;
    float4 color : COLOR;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
    float4 color : COLOR;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.position = input.position;
    output.color = input.color;

	return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
    return input.color;
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
