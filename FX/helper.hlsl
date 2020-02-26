
struct AppData
{
	float3 position : POSITION;
    float3 color: COLOR;
};

struct VSOut
{
	float4 color : COLOR;
    float4 position : SV_POSITION;
};

