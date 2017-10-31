Texture2D<unorm float4> inTex_bloom : register(t0);
Texture2D<unorm float4> inTex_bb : register(t1);
RWTexture2D<unorm float4> outTex : register(u0);

static const int3 dispatchSize = { 32, 40, 1 };
static const int3 groupSize = { 32, 23, 1 };

static const int kernelRadius = 32;


static const float bloomBaseMultiplier = 1;
static const float bloomFadeExponent = 2;
static const float bloomAdditiveColorStrengthMultiplier = .8f;

groupshared float3 groupMemory[groupSize.x * groupSize.y];

[numthreads(groupSize.x, groupSize.y, groupSize.z)]
void CS_main(int3 groupID : SV_GroupID, int groupIndex : SV_GroupIndex)
{
    int formattedGroupID = groupID.y * dispatchSize.x + groupID.x;
    int2 currentPixel = { formattedGroupID, groupIndex };

    groupMemory[groupIndex] = inTex_bloom[currentPixel];

    GroupMemoryBarrierWithGroupSync();

    float4 backBufferColor = inTex_bb[currentPixel];

    float3 blurredColor = { 0, 0, 0 };
    for (int offset = -kernelRadius; offset < kernelRadius + 1; offset++)
    {
        int kernelIndex = groupIndex + offset;
        kernelIndex = sign(kernelIndex) * kernelIndex +
                      2 * min(0, 719 - kernelIndex);

        blurredColor += groupMemory[kernelIndex];
    }
    blurredColor /= 2 * kernelRadius + 1;

    blurredColor =  bloomBaseMultiplier * blurredColor +
                    pow(blurredColor, bloomFadeExponent) +
                    bloomAdditiveColorStrengthMultiplier * (blurredColor.r + blurredColor.g + blurredColor.b) * blurredColor;

    outTex[currentPixel] = backBufferColor + float4(blurredColor, 1);
}