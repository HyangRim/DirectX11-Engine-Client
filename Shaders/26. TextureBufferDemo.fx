

Texture2DArray<float4> Input;
RWTexture2DArray<float4> Output;


//�̰� �� ���ϴ°�? 
//������ ������ �����ϱ�. Cell������. ���� ���� �̾. 
//3������ �迭 10 * 8 * 3 = 240�� thread

//(7,5,0)�׷� �������� ����, ���� �׷쿡���� �������� ����. 


//DispatchThreadID�� ��� �����忡�� �����ϴ�. 
//�� 3�������� ������? ���̴��� ���� �۾��� ���� �ؽ��Ĵ� 2���� �����Ͱ�
//�׷��� �׷� ���� ������ ���� �ʾ�����. �����Ѵ�. 
//�������� �� �ø� �� ����. ���������� 1024�� ������.
//�� �̻����� ������ �׷��� ������� ��. ��κ��� ���� ������ �й�� �ϳ��� �����尡 ��� �κ��� ����Ѵ�. 


[numthreads(32,32,1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    float4 color = Input.Load(int4(id, 0));
    
    //Output[id] = color;
    
    Output[id] = 1.0f - color;

}

technique11 T0
{
	//���� ���� �н��� �д�. 
   
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS()));

    }

};
