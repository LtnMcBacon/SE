static const float fogSize = 1;
static const float cycleTime = 12;


Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState samplerState : register(s0);


cbuffer timeBuffer : register(b0)
{
    float time;
};


static const float fogSpeed = 2.5 / cycleTime;
static const float cycleTime_quarter = cycleTime / 4;


struct PS_IN
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


float4 PS_main(PS_IN input) : SV_TARGET
{
    float clock = time % cycleTime;


    float2 uvOffset = input.uv - float2(12.5, 12.5);
    float2 uvOffset_signs = float2(sign(uvOffset.x), sign(uvOffset.y));

    uvOffset = normalize(uvOffset);
    uvOffset *= uvOffset;
    uvOffset *= uvOffset_signs;

    uvOffset *= clock;
    uvOffset *= fogSpeed;


    float4 albedoValues = albedoTexture.Sample(samplerState, (input.uv + uvOffset) / fogSize);
    float4 normalValues = normalTexture.Sample(samplerState, (input.uv + uvOffset) / fogSize);


    float3 albedoColor = albedoValues.rgb;
    float albedoAlpha = albedoValues.a;

    float3 normalColor = normalize(normalValues.rgb);

    float diffuseFactor = dot(float3(0, 0, 1), normalColor);


    float swapMultiplier = clock / cycleTime_quarter;

    float sm_fade = max(0, swapMultiplier - 2);
    bool sm_fade_active = sm_fade;

    swapMultiplier *= !sm_fade_active;
    swapMultiplier += sm_fade_active * (1 - sm_fade);

    swapMultiplier = saturate(swapMultiplier);
    float sm_transparency = 1 - swapMultiplier;
    bool sm_transparency_active = sm_transparency;

    swapMultiplier *= !sm_transparency_active;
    swapMultiplier += sm_transparency_active * (1 - sm_transparency * sm_transparency * sm_transparency);


    return float4(diffuseFactor * albedoColor, /*diffuseFactor * */swapMultiplier * input.opacity);
}
