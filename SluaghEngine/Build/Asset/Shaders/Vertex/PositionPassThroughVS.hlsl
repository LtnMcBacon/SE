cbuffer OncePerFrame : register(b1)
{
	float4x4 View;
	float4x4 ViewProj;
};


struct VS_IN
{
	float3 Pos : POSITION;	
};

struct VS_OUT
{
	float4 Pos : SV_Position;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), ViewProj);

	return output;
}