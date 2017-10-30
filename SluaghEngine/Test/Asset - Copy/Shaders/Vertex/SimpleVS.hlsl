
cbuffer OncePerFrame : register(b0)
{
	float4x4 ViewProj;
};

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
	float4 PosInW : WORLDPOS;
	float4 NormalInW : NORMALINW;
	float4 BinormalInW : BINORMALINW;
	float4 TangentInW : TANGENTINW;
	float2 Tex : TEXCOORD;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(mul(float4(input.Pos, 1), World[input.InstanceID]), ViewProj);
	output.PosInW = mul(float4(input.Pos, 1), World[input.InstanceID]);
	output.NormalInW = normalize(mul(float4(input.Normal, 0), World[input.InstanceID]));
	output.BinormalInW = normalize(mul(float4(input.Binormal, 0), World[input.InstanceID]));
	output.TangentInW = normalize(mul(float4(input.Tangent, 0), World[input.InstanceID]));
	output.Tex = input.Tex;

	return output;
}