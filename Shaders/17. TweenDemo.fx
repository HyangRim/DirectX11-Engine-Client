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

struct TweenFrameDesc
{
    float tweenDuration;
    float tweenRatio;
    float tweenSumTime;
    float padding;
    KeyframeDesc curr;
    KeyframeDesc next;
};

cbuffer TweenBuffer
{
    TweenFrameDesc TweenFrames;
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

    int animIndex[2];
    int currFrame[2];
    int nextFrame[2];
    
    float ratio[2];
    
    animIndex[0] = TweenFrames.curr.animIndex;
    currFrame[0] = TweenFrames.curr.currFrame;
    nextFrame[0] = TweenFrames.curr.nextFrame;
    ratio[0] = TweenFrames.curr.ratio;
    
    animIndex[1] = TweenFrames.next.animIndex;
    currFrame[1] = TweenFrames.next.currFrame;
    nextFrame[1] = TweenFrames.next.nextFrame;
    ratio[1] = TweenFrames.next.ratio;
    

    //�ִ� ����޴� �� 4��. 
	float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    
	matrix curr = 0;
    matrix next = 0;
	matrix transform = 0;

	for (int i = 0; i < 4; i++)
	{
		c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
		c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
		c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
		c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));

        
        //�� ���� ���� ����ؼ� ����� ����ϰ�.
		curr = matrix(c0, c1, c2, c3);
        
        n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
        n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
        n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
        n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
        
        //�� ���� ���� ����ؼ� ����� ����ϰ�.
        next = matrix(n0, n1, n2, n3);
        
        matrix result = lerp(curr, next, ratio[0]);
        
        //���� �ִϸ��̼� �ִ��� üũ
        if (animIndex[1] >= 0)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
            next = matrix(n0, n1, n2, n3);

            matrix nextResult = lerp(curr, next, ratio[1]);
            result = lerp(result, nextResult, TweenFrames.tweenRatio);
        }
        
        //����ġ�� ���ؼ� transform�� ������. 
		transform += mul(weights[i], curr);
	}

	return transform;
}

//00 Global���� �޾ƿ�. 
//pragma once�� �� ���� ������ �ǵ��� ��.

//matrix ToRootTransform
//�̷��� �ٷ� �־��༭ ������� ���� ������ ����?


//CBuffer�� ���� �����͸� ���� �� �ִ� �� �ƴϴ�. 



MeshOutput VS(VertexTextureNormalTangentBlend input)
{
    
    // W * V * P
    MeshOutput output;
    
    //TODO(Blend)
    Matrix m = GetAnimationWorldMatrix(input);
    
    output.position = mul(input.position, m);
    
    //�̰� �ϸ� ROOT�� ��ǥ��� �����. C -> ROOT�� 
    //output.position = mul(input.position, BoneTransforms[BoneIndex]);
    
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    
    //��ü�� �� ȸ���� ���� normal�� ������ �� ����. 
    output.normal = mul(input.normal, (float3x3)W);
    
    output.tangent = mul(input.tangent, (float3x3) W);

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
	//���� ���� �н��� �д�. 
    PASS_VP(P0, VS, PS)
    PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)
};
