static const float SCROLL_SPEED = 0.3;

static const float WAVE_MULTIPLIER = 0.12;
static const float WAVE_START_OFFSET = -0.25;
static const float WAVE_CYCLE_DURATION = 5;

static const float WAVE_TOP_TRESHOLD = 3;
static const float WAVE_MIDDLE_TRESHOLD = 1;

static const float WAVE_MIDDLE_MULTIPLIER_OFFSET = 2;
static const float WAVE_BOTTOM_MULTIPLIER_OFFSET = 8;


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
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float opacity : OPACITY;
};


VS_OUT VS_main(VS_IN input)
{
    VS_OUT output;


    float3 positionOffset = float3(fmod(input.position.x + 0.5, 1) - 0.5,
                                   0,
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


    bool isBottomVertex = input.uv.y < WAVE_MIDDLE_TRESHOLD;
    bool isMiddleVertex = !isBottomVertex && (input.uv.y < WAVE_TOP_TRESHOLD);

    offsetMultiplier *= 1 + isMiddleVertex * WAVE_MIDDLE_MULTIPLIER_OFFSET + isBottomVertex * WAVE_BOTTOM_MULTIPLIER_OFFSET;


    offsetMultiplier *= WAVE_MULTIPLIER;
    offsetMultiplier += WAVE_START_OFFSET * offsetMultiplier;


    output.position = mul(float4(input.position + offsetMultiplier * positionOffset, 1), viewProjectionMatrix);

    output.uv[0] = input.uv[0];
    output.uv[1] = input.uv[1] + SCROLL_SPEED * time;

    output.opacity = input.opacity;


    return output;
}
