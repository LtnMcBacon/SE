static const float fogSize = 1.f;

static const float fogPlaneSize = 25.f + 2.f * 10.f;


Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState samplerState : register(s0);


struct PS_IN
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


float4 PS_main(PS_IN input) : SV_TARGET
{
    float4 albedoValues = albedoTexture.Sample(samplerState, (fogPlaneSize / fogSize) * input.uv);
    float4 normalValues = normalTexture.Sample(samplerState, (fogPlaneSize / fogSize) * input.uv);

    return float4(albedoValues * dot(float3(0, 0, 1), normalValues.rgb));
}
