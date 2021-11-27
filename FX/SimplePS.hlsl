#include "helper.hlsl"

//the simplest pixel shader imageinable - just pass the input colour straight back
float4 main(VSOut IN) : SV_TARGET
{
	return IN.color;
}
