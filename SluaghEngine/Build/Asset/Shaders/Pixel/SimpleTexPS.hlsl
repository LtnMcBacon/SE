Texture2D DiffuseColor : register(t0);

SamplerState sampAni : register(s0);

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float4 PosInW : WORLDPOS;
	float4 NormalInW : NORMALINW;
	float4 BinormalInW : BINORMALINW;
	float4 TangentInW : TANGENTINW;
	float2 Tex : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_TARGET
{
	float3 dir = normalize(float3(0.0f, -2.5f, 1.0f));
	float lum = dot(input.NormalInW.xyz, -dir);
	float3 color =  DiffuseColor.Sample(sampAni, input.Tex)*lum;
	return float4(color, 1.0f);
}