#include "00. Global.fx"
#include "00. Light.fx"

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500

struct KeyframeDesc
{
    int animIndex;
    uint currFrame;
    uint nextFrame;
    float ratio;
    float sumTime;
    float speed;
    float2 padding;
    
};

cbuffer KeyframeBuffer
{
    KeyframeDesc Keyframes;
};

cbuffer BoneBuffer
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
};

uint BoneIndex;
Texture2DArray TransformMap;


matrix GetAnimationWorldMatrix(VertexTextureNormalTangentBlend input)
{
	float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
	float weights[4] = { input.blendWeights.x, input.blendWeights.y, input.blendWeights.z, input.blendWeights.w };

	int animIndex = Keyframes.animIndex;
	int currFrame = Keyframes.currFrame;
	int nextFrame = Keyframes.nextFrame;
    
    float ratio = Keyframes.ratio;
    

    //최대 영향받는 뼈 4개. 
	float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    
	matrix curr = 0;
    matrix next = 0;
	matrix transform = 0;

	for (int i = 0; i < 4; i++)
	{
		c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame, animIndex, 0));
		c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame, animIndex, 0));
		c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame, animIndex, 0));
		c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame, animIndex, 0));

        
        //그 영향 값을 계산해서 행렬을 계산하고.
		curr = matrix(c0, c1, c2, c3);
        
        n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame, animIndex, 0));
        n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame, animIndex, 0));
        n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame, animIndex, 0));
        n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame, animIndex, 0));
        
        //그 영향 값을 계산해서 행렬을 계산하고.
        next = matrix(n0, n1, n2, n3);
        
        matrix result = lerp(curr, next, ratio);
        
        //가중치를 곱해서 transform을 결정함. 
		transform += mul(weights[i], curr);
	}

	return transform;
}

//00 Global에서 받아옴. 
//pragma once는 한 번만 컴파일 되도록 함.

//matrix ToRootTransform
//이렇게 바로 넣어줘서 계산하지 않은 이유가 뭔가?


//CBuffer은 많은 데이터를 넣을 수 있는 게 아니다. 



MeshOutput VS(VertexTextureNormalTangentBlend input)
{
    
    // W * V * P
    MeshOutput output;
    
    //TODO(Blend)
    Matrix m = GetAnimationWorldMatrix(input);
    
    output.position = mul(input.position, m);
    
    //이걸 하면 ROOT의 좌표계로 변경됨. C -> ROOT로 
    //output.position = mul(input.position, BoneTransforms[BoneIndex]);
    
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    
    //물체의 그 회전에 따라 normal이 딸라질 수 있음. 
    output.normal = mul(input.normal, (float3x3)W);
    
    output.tangent = mul(input.tangent, (float3x3) W);

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

//Emissive
//보통 외곽선 구할 때 사용(림라이트)


float4 PS(MeshOutput input) : SV_TARGET
{
    //ComputeNormalMapping(input.normal, input.tangent, input.uv);
    
    //float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);
    
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    return color;
}

float4 PS_RED(MeshOutput input) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}

technique11 T0
{
	//여러 개의 패스를 둔다. 
    PASS_VP(P0, VS, PS)
    PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)
};
