static const float fogSize = 1.f;


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
    float4 albedoValues = albedoTexture.Sample(samplerState, input.uv / fogSize);
    float4 normalValues = normalTexture.Sample(samplerState, input.uv / fogSize);

    return float4(dot(float3(0, 0, 1), normalValues.rgb) * albedoValues.rgb, dot(float3(0, 0, 1), normalValues.rgb) * input.opacity);
}
