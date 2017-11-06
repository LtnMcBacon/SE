
cbuffer LightDataBuffer : register(b0)
{
	float4 LightPosWorld;
	float LightRange;
};

struct PS_IN
{
	float4 PosClipSpace : SV_Position;
	uint cubeFaceIndex : SV_RenderTargetArrayIndex;
	float3 LightDirWorld : DIRECTION;
};

float PS_main(PS_IN input) : SV_Depth
{
	return length(input.LightDirWorld) / LightRange;
}