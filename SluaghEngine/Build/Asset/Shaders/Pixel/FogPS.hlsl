static const float fogSize = 1.f;
static const float fogSpeed = 0.1f;


Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState samplerState : register(s0);


cbuffer timeBuffer : register(b0)
{
    float time;
};


struct PS_IN
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


float4 PS_main(PS_IN input) : SV_TARGET
{
    float4 albedoValues = albedoTexture.Sample(samplerState, input.uv / fogSize + fogSpeed * time);
    float4 normalValues = normalTexture.Sample(samplerState, input.uv / fogSize + fogSpeed * time);


    float3 albedoColor = albedoValues.rgb;
    float albedoAlpha = albedoValues.a;

    float3 normalColor = normalize(normalValues.rgb);

    float diffuseFactor = dot(float3(0, 0, 1), normalColor);


    return float4(diffuseFactor * albedoColor, diffuseFactor * input.opacity);
}
