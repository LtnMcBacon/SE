static const int2 RESOLUTION = { 640, 360 };


static const int3 DISPATCH_SIZE = { 16, 15, 1 };
static const int3 GROUP_SIZE = { 32, 32, 1 };

static const int KERNEL_RADIUS = 32;
static const int KERNEL_STEPS = 4;


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


    if (threadIndex >= KERNEL_RADIUS && threadIndex < GROUP_THREAD_COUNT - KERNEL_RADIUS)
    {
        float3 bloomSteps[1 + KERNEL_STEPS];
        float bloomSteps_sum[1 + KERNEL_STEPS];


        int kernelStep;
        for (kernelStep = KERNEL_STEPS; kernelStep > 0; kernelStep--)
        {
            bloomSteps[kernelStep] = float3(0, 0, 0);

            int limit = -KERNEL_RADIUS / pow(2, 1 + KERNEL_STEPS - kernelStep);
            limit *= min(1, kernelStep - 1);

            for (int pixelOffset = -KERNEL_RADIUS / pow(2, KERNEL_STEPS - kernelStep); pixelOffset < limit; pixelOffset++)
            {
                int offsetRowIndex = pixelIndex_2d.x + pixelOffset;
                int kernelOffset = min(0, offsetRowIndex) * -2;

                bloomSteps[kernelStep] += groupMemory[threadIndex + pixelOffset + kernelOffset];
            }
        }


        bloomSteps[0] = groupMemory[threadIndex];


        for (kernelStep = 1; kernelStep < 1 + KERNEL_STEPS; kernelStep++)
        {
            int init = KERNEL_RADIUS / pow(2, 1 + KERNEL_STEPS - kernelStep);
            init *= min(1, kernelStep - 1);
            init = max(1, init);

            for (int pixelOffset = init; pixelOffset <= KERNEL_RADIUS / pow(2, KERNEL_STEPS - kernelStep); pixelOffset++)
            {
                int offsetRowIndex = pixelIndex_2d.x + pixelOffset;
                int kernelOffset = max(0, offsetRowIndex - (RESOLUTION.x - 1)) * -2;

                bloomSteps[kernelStep] += groupMemory[threadIndex + pixelOffset + kernelOffset];
            }
        }


        for (kernelStep = 0; kernelStep < KERNEL_STEPS; kernelStep++)
        {
            bloomSteps_sum[kernelStep] = bloomSteps[kernelStep].r + bloomSteps[kernelStep].g + bloomSteps[kernelStep].b;

            bloomSteps[kernelStep + 1] /= 2 * KERNEL_RADIUS / pow(2, KERNEL_STEPS - (kernelStep + 1));
            bloomSteps[kernelStep + 1] += bloomSteps[kernelStep];
        }
        bloomSteps_sum[KERNEL_STEPS] = bloomSteps[KERNEL_STEPS].r + bloomSteps[KERNEL_STEPS].g + bloomSteps[KERNEL_STEPS].b;


        int optimalStep = 0;
        float optimalStep_sum = 0;

        for (kernelStep = 0; kernelStep < 1 + KERNEL_STEPS; kernelStep++)
        {
            if (bloomSteps_sum[kernelStep] > optimalStep_sum)
            {
                optimalStep = kernelStep;

                optimalStep_sum = bloomSteps_sum[kernelStep];
            }
        }


        outTex[pixelIndex_2d] = float4(bloomSteps[optimalStep], 1);
    }
}
