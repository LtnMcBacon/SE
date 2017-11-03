static const int2 RESOLUTION = { 640, 360 };


static const int3 DISPATCH_SIZE = { 16, 15, 1 };
static const int3 GROUP_SIZE = { 32, 32, 1 };

static const int KERNEL_RADIUS = 32;


//static const float BLOOM_BASE_MULTIPLIER = 5;
//static const float BLOOM_FADE_EXPONENT = 2;
//static const float BLOOM_ADDITIVE_COLOR_STRENGTH_MULTIPLIER = 0.8f;

cbuffer BloomProperties : register(b0)
{
	float BLOOM_BASE_MULTIPLIER;
	float BLOOM_FADE_EXPONENT;
	float BLOOM_ADDITIVE_COLOR_STRENGTH_MULTIPLIER;
	float BLOOM_AT;
};

Texture2D<unorm float4> inTex_bloom : register(t0);
//Texture2D<unorm float4> inTex_bb : register(t1);
RWTexture2D<unorm float4> outTex : register(u0);

static const int GROUP_THREAD_COUNT = { GROUP_SIZE.x * GROUP_SIZE.y * GROUP_SIZE.z };


groupshared float3 groupMemory[GROUP_THREAD_COUNT];


[numthreads(GROUP_SIZE.x, GROUP_SIZE.y, GROUP_SIZE.z)]
void CS_main(int3 groupIndex_3d : SV_GroupID, int threadIndex : SV_GroupIndex)
{
    int groupIndex = (groupIndex_3d.z * DISPATCH_SIZE.x * DISPATCH_SIZE.y + groupIndex_3d.y * DISPATCH_SIZE.x + groupIndex_3d.x);

    int pixelIndex = groupIndex * GROUP_THREAD_COUNT - (groupIndex * 2 * KERNEL_RADIUS) + threadIndex - KERNEL_RADIUS;
    int2 pixelIndex_2d = { pixelIndex / RESOLUTION.y, pixelIndex % RESOLUTION.y };


    groupMemory[threadIndex] = inTex_bloom[pixelIndex_2d];

    GroupMemoryBarrierWithGroupSync();


    if (threadIndex >= KERNEL_RADIUS && threadIndex < GROUP_THREAD_COUNT - KERNEL_RADIUS)
    {
      //  float4 backBufferColor = inTex_bb[pixelIndex_2d];


        float3 blurredColor = { 0, 0, 0 };
        for (int pixelOffset = -KERNEL_RADIUS; pixelOffset <= KERNEL_RADIUS; pixelOffset++)
        {
            int offsetColumnIndex = pixelIndex_2d.y + pixelOffset;
            int kernelOffset = (min(0, offsetColumnIndex) + max(0, offsetColumnIndex - (RESOLUTION.y - 1))) * -2;

            blurredColor += groupMemory[threadIndex + pixelOffset + kernelOffset];
        }
        blurredColor /= 2 * KERNEL_RADIUS + 1;


        blurredColor = BLOOM_BASE_MULTIPLIER * blurredColor +
    pow(blurredColor, BLOOM_FADE_EXPONENT) +
    BLOOM_ADDITIVE_COLOR_STRENGTH_MULTIPLIER * (blurredColor.r + blurredColor.g + blurredColor.b) * blurredColor;


        outTex[pixelIndex_2d] = float4(blurredColor, 1);
    }
}
