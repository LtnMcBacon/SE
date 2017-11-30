cbuffer LightShadowDataBuffer : register(b0)
{
	float4 LightPosWorld;
	float4 LightRange;
};

cbuffer SkinnedOncePerObject : register(b1)
{
	float4x4 World[8];
};

struct BoneTransformsStruct
{
	float4x4 t[30];
};
cbuffer VS_SKINNED_DATA: register (b2) {

	BoneTransformsStruct BoneTransforms[8];
};

struct VS_IN
{
	float3 Pos : POSITION;	
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float3 Binormal : BINORMAL;
	float3 Tangent : TANGENT;
	float4 Weight : BLENDWEIGHT;
	uint4 BoneIndices : BLENDINDICES;
	uint InstanceID : SV_InstanceID;
};

struct VS_OUT
{
	float4 PosWorld : SV_POSITION;
	float4 LightDirWorld : DIRECTION;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float3 position = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i) // loop trough the 4 weights
	{
		position += input.Weight[i] * mul(float4(input.Pos, 1.0f), BoneTransforms[input.InstanceID].t[input.BoneIndices[i]]).xyz; // the vertex position is affected by the joint movement and influenced by the corresponding weight	
	}
	output.PosWorld = mul(float4(position, 1), World[input.InstanceID]);
	output.LightDirWorld = output.PosWorld - LightPosWorld;
	
	return output;
}