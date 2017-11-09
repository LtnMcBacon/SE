
cbuffer LightVPBuffer : register(b0)
{
	float4x4 LightVP[6];
};

struct GS_IN
{
	float4 PosWorld : SV_Position;
	float4 LightDirWorld : DIRECTION;
};

struct GS_OUT
{
	float4 PosClipSpace : SV_Position;
	uint cubeFaceIndex : SV_RenderTargetArrayIndex;
	float3 LightDirWorld : DIRECTION;
};

[instance(6)]
[maxvertexcount(3)]
void GS_main(uint cubeFaceID : SV_GSInstanceID, triangle GS_IN input[3], inout TriangleStream<GS_OUT> outputStream)
{
	GS_OUT output;
	output.cubeFaceIndex = cubeFaceID;
	
	for (uint i = 0; i < 3; ++i)
	{
		output.LightDirWorld = input[i].LightDirWorld;
		output.PosClipSpace = mul(input[i].PosWorld,LightVP[cubeFaceID]);
		outputStream.Append(output);
	}
	
	outputStream.RestartStrip();
}