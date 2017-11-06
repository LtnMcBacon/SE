static const int2 RESOLUTION = { 640, 360 };


static const int3 DISPATCH_SIZE = { 16, 15, 1 };
static const int3 GROUP_SIZE = { 32, 32, 1 };

static const int KERNEL_RADIUS = 32;


Texture2D<unorm float4> inTex : register(t0);
RWTexture2D<unorm float4> outTex : register(u0);

static const int GROUP_THREAD_COUNT = { GROUP_SIZE.x * GROUP_SIZE.y * GROUP_SIZE.z };


groupshared float3 groupMemory[GROUP_THREAD_COUNT];


[numthreads(GROUP_SIZE.x, GROUP_SIZE.y, GROUP_SIZE.z)]
void CS_main(int3 groupIndex_3d : SV_GroupID, int threadIndex : SV_GroupIndex)
{
    int groupIndex = (groupIndex_3d.z * DISPATCH_SIZE.x * DISPATCH_SIZE.y + groupIndex_3d.y * DISPATCH_SIZE.x + groupIndex_3d.x);

    int pixelIndex = groupIndex * GROUP_THREAD_COUNT - (groupIndex * 2 * KERNEL_RADIUS) + threadIndex - KERNEL_RADIUS;
    int2 pixelIndex_2d = { pixelIndex % RESOLUTION.x, pixelIndex / RESOLUTION.x };


    groupMemory[threadIndex] = inTex[pixelIndex_2d];

    GroupMemoryBarrierWithGroupSync();


    if(threadIndex >= KERNEL_RADIUS && threadIndex < GROUP_THREAD_COUNT - KERNEL_RADIUS)
    {
        float3 blurredColor = { 0, 0, 0 };
        for (int pixelOffset = -KERNEL_RADIUS; pixelOffset <= KERNEL_RADIUS; pixelOffset++)
        {
            int offsetRowIndex = pixelIndex_2d.x + pixelOffset;
            int kernelOffset = (min(0, offsetRowIndex) + max(0, offsetRowIndex - (RESOLUTION.x - 1))) * -2;

            blurredColor += groupMemory[threadIndex + pixelOffset + kernelOffset];
        }
        blurredColor /= 2 * KERNEL_RADIUS + 1;


        outTex[pixelIndex_2d] = float4(blurredColor, 1);
    }
}
