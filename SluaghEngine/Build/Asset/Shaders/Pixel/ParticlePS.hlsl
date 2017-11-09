Texture2D particleTex : register(t0);
SamplerState samp : register(s0);

struct PS_IN {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float opacity : OPACITY;
};

float4 PS_main(PS_IN input) : SV_TARGET
{
	float4 sampledTex;
	sampledTex = particleTex.Sample(samp, input.tex);
	sampledTex.a *= input.opacity;
	clip(sampledTex.a - 0.1f);
	return sampledTex;
}