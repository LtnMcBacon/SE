static const float fogSize = 1;
static const float fogSpeed = 0.3;


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
    float2 scrollingUv = float2(input.uv.x, input.uv.y + fogSpeed * time);

    float4 albedoValues = albedoTexture.Sample(samplerState, scrollingUv / fogSize);
    float4 normalValues = normalTexture.Sample(samplerState, scrollingUv / fogSize);


    float3 albedoColor = albedoValues.rgb;
    float albedoAlpha = albedoValues.a;

    float3 normalColor = normalize(normalValues.rgb);

    float diffuseFactor = dot(float3(0, 0, 1), normalColor);


    return float4(diffuseFactor * albedoColor, /*diffuseFactor * */input.opacity);
}
