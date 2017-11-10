cbuffer OncePerRenderer : register(b0)
{
	float4x4 Proj;
};

cbuffer OncePerFrame : register(b1)
{
	float4x4 View;
	float4x4 ViewProj;
};


cbuffer DebugLineW : register(b2)
{
	float4x4 World;
};
struct VS_IN
{
	float3 Pos : POSITION;	
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = mul(mul(float4(input.Pos, 1), World), ViewProj);
	return output;
}