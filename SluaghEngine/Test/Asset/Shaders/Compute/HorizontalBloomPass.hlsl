Texture2D<unorm float4> inTex : register(t0);
RWTexture2D<unorm float4> outTex : register(u0);

static const int2 screenSize = {1280, 720};
static const int3 dispatchSize = { 32, 45, 1 };
static const int3 groupSize = { 32, 21, 1 };

static const int kernelRadius = 32;

groupshared float3 groupMemory[groupSize.x * groupSize.y + kernelRadius];

[numthreads(groupSize.x, groupSize.y, groupSize.z)]
void CS_main(int3 groupID : SV_GroupID, int groupIndex : SV_GroupIndex)
{
    int2 formattedGroupID = { groupID.x % 2, groupID.y * (dispatchSize.x / 2) + groupID.x / 2 };
    int2 currentPixel = { formattedGroupID.x * (screenSize.x/2 - kernelRadius - 1) + groupIndex, formattedGroupID.y };

    groupMemory[groupIndex] = inTex[currentPixel];

    GroupMemoryBarrierWithGroupSync();

    float3 blurredColor = { 0, 0, 0 };
    for (int offset = -kernelRadius; offset < kernelRadius + 1; offset++)
    {
        int kernelIndex = groupIndex + offset;
        kernelIndex = ((formattedGroupID.x - 1) * -1) * sign(kernelIndex) * kernelIndex +
                      formattedGroupID.x * (kernelIndex + 2 * min(0, (screenSize.x/2 + kernelRadius - 1) - kernelIndex));

        blurredColor += groupMemory[kernelIndex];
    }
    blurredColor /= 2 * kernelRadius + 1;

    if ((formattedGroupID.x == 0 && groupIndex < screenSize.x/2) || (formattedGroupID.x == 1 && groupIndex > kernelRadius - 1))
        outTex[currentPixel] = float4(blurredColor, 1);
}