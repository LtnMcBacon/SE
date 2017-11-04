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
	//float3 dir = float3(-1.0f, -0.5f, 0.0f);
	//float lum = dot(input.NormalInW.xyz, -dir);
	float3 color = float3(0.9f, 0.1f, 0.35f);

	return float4(color, 1.0f);
}