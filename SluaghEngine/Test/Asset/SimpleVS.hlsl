cbuffer worldMatrix : register(b1)
{
	float4x4 World;
};

cbuffer viewMatrix : register(b0)
{
	float4x4 viewproj;
};

struct VS_IN
{
	float3 Pos : POSITION;	
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 PosInW : WORLDPOS;
	float4 NormalInW : NORMALINW;
	float2 Tex : TEXCOORD;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(mul(float4(input.Pos, 1), World), viewproj);
	output.PosInW = mul(float4(input.Pos, 1), World);
	output.NormalInW = normalize(mul(float4(input.Normal, 1), World));
	output.Tex = input.Tex;

	return output;
}