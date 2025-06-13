#ifndef _GLOBAL_FX_
#define _GLOBAL_FX_


////////////////
//Const Buffer//
////////////////

cbuffer GlobalBuffer
{
    matrix V;
    matrix P;
    matrix VP;
    matrix Vinv;
};

cbuffer TransformBuffer
{
    matrix W;
};


////////////////
//Vertex Data //
////////////////

struct Vertex
{
    float4 position : POSITION;
};

struct VertexTexture
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};


struct VertexColor
{
    float4 position : POSITION;
    float4 color : COLOR;
};


struct VertexTextureNormal
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexTextureTangentNormal
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexTextureNormalTangentBlend
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 blendIndices : BLEND_INDICES;
    float4 blendWeights : BLEND_WEIGHTS;
};
////////////////
//VertexOutput//
////////////////

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct MeshOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

////////////////
//SamplerState//
////////////////

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

///////////////////
//RasterizerState//
///////////////////

RasterizerState FillModeWireFrame
{
    FillMode = WireFrame;
};

RasterizerState FrontCounterClickwiseTrue
{
    FrontCounterClockwise = true;
};

///////////////////
//     Macro     //
///////////////////

#define PASS_VP(name, vs, ps)                           \
pass name                                               \
{                                                       \
	SetVertexShader(CompileShader(vs_5_0, vs()));       \
	SetPixelShader(CompileShader(ps_5_0, ps()));        \
}                                 

#define PASS_RS_VP(name, rs, vs, ps)                    \
pass name                                               \
{                                                       \
    SetRasterizerState(rs);                             \
	SetVertexShader(CompileShader(vs_5_0, vs()));       \
	SetPixelShader(CompileShader(ps_5_0, ps()));        \
}       

///////////////////
//    Function   //
///////////////////


//월드 좌표계에서는 View변환 행렬을 구하면 월드의 역행렬임. 
//회전이 들어가면 Right, Up, Look이 섞여 들어가기 떄문에. 


float3 CameraPosition()
{
    return -Vinv._41_42_43;
}


#endif