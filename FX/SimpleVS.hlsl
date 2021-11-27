#include "helper.hlsl"

//structure matching the one on the CPU, this is a contstant register
cbuffer PerObject : register(b0)
{
	float4x4 projectionMatrix; //master work->view->projection matrix from the cpu
}

//vertex shader, input is from the geometry buffer
VSOut main( AppData IN )
{
    VSOut OUT;

	//make sure this multiplication is the right way round!
	//matrix multiplaction is not commutative (c=A*B, but c!=B*A)
	OUT.position = mul(projectionMatrix, float4(IN.position, 1.0f));
	OUT.color = float4(IN.color, 1.0f);

    return OUT;
}

