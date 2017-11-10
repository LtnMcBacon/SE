
RWTexture2D<float4> outTex : register(u0);

static const int3 dispatchSize = { 4, 4, 1 };
static const int3 groupSize = { 32, 32, 1 };

[numthreads(groupSize.x, groupSize.y, groupSize.z)]
void CS_main(int3 groupID : SV_GroupID, int3 id : SV_DispatchThreadID)
{
	outTex[id.xy] = float4(1.0f, 0.0f, 0.0f,1.0f);
}