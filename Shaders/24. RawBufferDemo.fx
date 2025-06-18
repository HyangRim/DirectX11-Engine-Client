


RWByteAddressBuffer Output; // UAV

struct ComputeInput
{
    uint3 groupID : SV_GroupID;
    uint3 groupThreadID : SV_GroupThreadID;
    uint3 dispatchThreadID : SV_DispatchThreadID;
    uint groupIndex : SV_GroupIndex;
};


//�̰� �� ���ϴ°�? 
//������ ������ �����ϱ�. Cell������. ���� ���� �̾. 
//3������ �迭 10 * 8 * 3 = 240�� thread

//(7,5,0)�׷� �������� ����, ���� �׷쿡���� �������� ����. 


//DispatchThreadID�� ��� �����忡�� �����ϴ�. 
[numthreads(10,8,3)]
void CS(ComputeInput input)
{
    uint index = input.groupIndex;
    uint outAddress = index * 10 * 4;
    
    Output.Store3(outAddress + 0, input.groupID);
    Output.Store3(outAddress + 12, input.groupThreadID);
    Output.Store3(outAddress + 24, input.dispatchThreadID);
    Output.Store(outAddress + 36, input.groupIndex);
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
