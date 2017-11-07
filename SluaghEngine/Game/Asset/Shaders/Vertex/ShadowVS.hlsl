cbuffer LightViewProj : register(b0)
{
	float4x4 lViewProj;
}

cbuffer OncePerObject : register(b1)
{
	float4x4 World[256];
};

cbuffer InversWorld : register(b2)
{
	float4x4 invers[256];
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
	float4 Pos : SV_POSITION;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	// Transform vertex into light space
	output.Pos = mul(mul(float4(input.Pos, 1), World[input.InstanceID]), lViewProj);
	
	return output;
}