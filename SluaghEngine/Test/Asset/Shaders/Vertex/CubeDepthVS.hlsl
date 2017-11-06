cbuffer LightWorldPos
{
	float4 LightPosWorld;
}

cbuffer OncePerObject : register(b1)
{
	float4x4 World[256];
};

struct VS_IN
{
	float3 Pos : POSITION;	
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float3 Binormal : BINORMAL;
	float3 Tangent : TANGENT;
	uint InstanceID : SV_InstanceID;
};

struct VS_OUT
{
	float4 PosWorld : SV_POSITION;
	float4 LightDirWorld : DIRECTION;
};



VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.PosWorld = mul(float4(input.Pos, 1), World[input.InstanceID]);
	output.LightDirWorld = output.PosWorld - LightPosWorld;

	return output;
}