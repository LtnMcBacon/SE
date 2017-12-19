static const float FOG_SIZE = 5;


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
    float4 albedoValues = albedoTexture.Sample(samplerState, input.uv / FOG_SIZE);
    float4 normalValues = normalTexture.Sample(samplerState, input.uv / FOG_SIZE);


    float3 albedoColor = albedoValues.rgb;
    float albedoAlpha = albedoValues.a;

    float3 normalColor = normalValues.rgb;

    float diffuseFactor = dot(float3(0, 0, 1), normalColor);


    return float4(diffuseFactor * albedoColor, 1.333 * input.opacity);
}
