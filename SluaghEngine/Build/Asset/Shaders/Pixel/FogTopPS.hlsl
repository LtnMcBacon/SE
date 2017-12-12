static const float FOG_SIZE = 1;

//static const uint2 WINDOW_RESOLUTION = { 1280, 720 };
//static const float VIEW_ROTATION = 55;
//static const float CENTER_ATTRACTION_STRENGTH = 1;


Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState samplerState : register(s0);


//static const float2 WINDOW_RESOLUTION_HALF = { WINDOW_RESOLUTION.x / 2.f, WINDOW_RESOLUTION.y / 2.f };
//static const float VIEW_ROTATION_INVERSE_SINE = sin(-(3.1416 / 180 * VIEW_ROTATION));
//static const float VIEW_ROTATION_INVERSE_COSINE = cos(-(3.1416 / 180 * VIEW_ROTATION));


struct PS_IN
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


float4 PS_main(PS_IN input) : SV_TARGET
{
    //float2 centerOffset = { input.position.x - WINDOW_RESOLUTION_HALF.x, (WINDOW_RESOLUTION.y - input.position.y) - WINDOW_RESOLUTION_HALF.y };
    //centerOffset = normalize(centerOffset);

    //float centerOffset_oldX = centerOffset.x;

    //centerOffset.x = VIEW_ROTATION_INVERSE_COSINE * centerOffset.x - VIEW_ROTATION_INVERSE_SINE * centerOffset.y;
    //centerOffset.y = VIEW_ROTATION_INVERSE_SINE * centerOffset_oldX + VIEW_ROTATION_INVERSE_COSINE * centerOffset.y;

    //uvOffset += CENTER_ATTRACTION_STRENGTH * centerOffset;


    float4 albedoValues = albedoTexture.Sample(samplerState, input.uv / FOG_SIZE);
    float4 normalValues = normalTexture.Sample(samplerState, input.uv / FOG_SIZE);


    float3 albedoColor = albedoValues.rgb;
    float albedoAlpha = albedoValues.a;

    float3 normalColor = normalValues.rgb;

    float diffuseFactor = dot(float3(0, 0, 1), normalColor);


    return float4(diffuseFactor * albedoColor, /*diffuseFactor * */input.opacity);
}
