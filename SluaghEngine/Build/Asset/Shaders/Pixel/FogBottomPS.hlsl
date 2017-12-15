static const float FOG_SIZE = 5;

static const float PROXIMITY_FADE_RANGE = 0.00015;
static const float PROXIMITY_FADE_TRESHOLD_RANGE = 0.0003;
static const float PROXIMITY_FADE_MAX_DISTANCE = 0.9988;


Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D depthTexture : register(t2);

SamplerState samplerState : register(s0);


struct PS_IN
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


float4 PS_main(PS_IN input) : SV_TARGET
{
    float4 depthValues = depthTexture.Sample(samplerState, float2(input.position.x / 1280, input.position.y / 720));

    float depthDifference = depthValues.r - input.position.z;
    clip(depthDifference);


    float4 albedoValues = albedoTexture.Sample(samplerState, input.uv / FOG_SIZE);
    float4 normalValues = normalTexture.Sample(samplerState, input.uv / FOG_SIZE);


    float3 albedoColor = albedoValues.rgb;
    float albedoAlpha = albedoValues.a;

    float3 normalColor = normalValues.rgb;


    float diffuseFactor = dot(float3(0, 0, 1), normalColor);


    float depthFadeMultiplier = min(PROXIMITY_FADE_RANGE, depthDifference) / PROXIMITY_FADE_RANGE;

    float depthFadeMultiplier_inverse = 1 - depthFadeMultiplier;
    depthFadeMultiplier_inverse *= saturate(max(0, PROXIMITY_FADE_MAX_DISTANCE - depthValues.r) / PROXIMITY_FADE_TRESHOLD_RANGE);

    depthFadeMultiplier = 1 - depthFadeMultiplier_inverse;


    return float4(diffuseFactor * albedoColor, depthFadeMultiplier * input.opacity);
}
