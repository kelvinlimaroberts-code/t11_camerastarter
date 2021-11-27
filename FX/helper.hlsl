
//matches the data coming from the CPU at the top of the graphics pipeline
//this is an untransformed vertex
struct AppData 
{
	float3 position : POSITION;
    float3 color: COLOR;
}; 

//after the vertex shader function runs we need a new vertex format to
//hold a transformed vertex
struct VSOut
{
	float4 color : COLOR;
    float4 position : SV_POSITION;
};

//these two are alternates where the pixel shader hard codes the colour
//and therefore there's no need to store a colour in the vertex, in theory
//that would make rendering slightly faster
struct AppData2
{
	float3 position : POSITION;
};

struct VSOut2
{
	float4 position : SV_POSITION;
};

