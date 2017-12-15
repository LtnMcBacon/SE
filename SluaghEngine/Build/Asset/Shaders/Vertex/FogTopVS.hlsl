static const float WAVE_RANGE = 0.2;
static const float WAVE_START_OFFSET = -0.25;
static const float WAVE_CYCLE_DURATION = 5;

static const float WAVE_OUTSIDE_OPACITY_TRESHOLD = 0.995;
static const float WAVE_OUTSIDE_MULTIPLIER = 0.4;

static const float WAVE_TOP_TRESHOLD = 3;
static const float WAVE_RIM_MULTIPLIER_OFFSET = 1;


static const float WAVE_CYCLE_DURATION_HALF = WAVE_CYCLE_DURATION / 2;


cbuffer OncePerFrame : register(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
};


cbuffer fog_timeBuffer : register(b1)
{
    float time;
};


struct VS_IN
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};

struct VS_OUT
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


VS_OUT VS_main(VS_IN input)
{
    VS_OUT output;


    float3 positionOffset = float3(fmod(input.position.x + 0.5, 1) - 0.5,
                                   (input.opacity > WAVE_OUTSIDE_OPACITY_TRESHOLD) * WAVE_OUTSIDE_MULTIPLIER,
                                   fmod(input.position.z + 0.5, 1) - 0.5);


    float offsetMultiplier = fmod(input.position.x - input.position.y + input.position.z + time, WAVE_CYCLE_DURATION)
                             / WAVE_CYCLE_DURATION_HALF;

    offsetMultiplier -= max(0, offsetMultiplier - 1) * 2;


    bool shouldFadeOut = (offsetMultiplier > 0.5);
    float shouldFadeOut_half = shouldFadeOut / 2.f;
    float shouldFadeIn_multiplier = !shouldFadeOut - shouldFadeOut;

    float interpolatedOffset = shouldFadeOut + shouldFadeIn_multiplier * (offsetMultiplier - shouldFadeOut_half) * 2;
    interpolatedOffset *= interpolatedOffset;

    offsetMultiplier = shouldFadeOut_half + (shouldFadeOut + shouldFadeIn_multiplier * interpolatedOffset) / 2;


    bool isRimVertex = input.uv.y < WAVE_TOP_TRESHOLD;

    offsetMultiplier *= 1 + isRimVertex * WAVE_RIM_MULTIPLIER_OFFSET;


    offsetMultiplier *= WAVE_RANGE;
    offsetMultiplier += WAVE_START_OFFSET * offsetMultiplier;


    output.position = mul(float4(input.position + offsetMultiplier * positionOffset, 1), viewProjectionMatrix);

    output.uv[0] = input.uv[0];
    output.uv[1] = input.uv[1];

    output.opacity = input.opacity;


    return output;
}
