Texture2D particleTex : register(t0);
SamplerState samp : register(s0);
cbuffer bloomCBuffer : register(b0)
{
	uint bloomCheck;
};

cbuffer BloomProperties : register(b1)
{
	float BLOOM_BASE_MULTIPLIER;
	float BLOOM_FADE_EXPONENT;
	float BLOOM_ADDITIVE_COLOR_STRENGTH_MULTIPLIER;
	float BLOOM_AT;
};
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float opacity : OPACITY;
};
struct PS_OUT
{
	float4 backBuffer: SV_TARGET0;
	float4 bloomBuffer: SV_TARGET1;
	
};
PS_OUT PS_main(PS_IN input) : SV_TARGET
{

	float4 sampledTex;
	sampledTex = particleTex.Sample(samp, input.tex);
	sampledTex.a *= input.opacity;
	clip(sampledTex.a - 0.1f);
	
	PS_OUT output;
	output.backBuffer = sampledTex;
	output.bloomBuffer = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if(bloomCheck == 1)
	{	
		output.bloomBuffer = float4(0.0f, 0.0f, 0.0f, 1.0f);
		if (output.backBuffer.r > BLOOM_AT) output.bloomBuffer.r = output.backBuffer.r * output.backBuffer.r;
		if (output.backBuffer.g > BLOOM_AT) output.bloomBuffer.g = output.backBuffer.g * output.backBuffer.g;
		if (output.backBuffer.b > BLOOM_AT) output.bloomBuffer.b = output.backBuffer.b * output.backBuffer.b;
	}

	
	return output;
}