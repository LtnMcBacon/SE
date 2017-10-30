cbuffer OncePerFrame : register(b0)
{
	float4x4 ViewProj;
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
	uint InstanceId : SV_InstanceID;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 PosInW : WORLDPOS;
	float4 NormalInW : NORMALINW;
	float4 BinormalInW : BINORMALINW;
	float4 TangentInW : TANGENTINW;
	float2 Tex : TEXCOORD;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	// Perform vertex blending
	
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 normal = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i) // loop trough the 4 weights
	{

		position += input.Weight[i] * mul(float4(input.Pos, 1.0f), BoneTransforms[input.InstanceId].t[input.BoneIndices[i]]).xyz; // the vertex position is affected by the joint movement and influenced by the corresponding weight

		normal += input.Weight[i] * mul(input.Normal, (float3x3)BoneTransforms[input.InstanceId].t[input.BoneIndices[i]]); // we make it 3x3 matrix to skip homogenus and take away translation.
	}
	
	output.Pos = mul(mul(float4(position, 1), World[input.InstanceId]), ViewProj);
	output.PosInW = mul(float4(normal, 1), World[input.InstanceId]);
	
	output.NormalInW = normalize(mul(float4(normal, 1), World[input.InstanceId]));
		output.BinormalInW = normalize(mul(float4(input.Binormal, 0), World[input.InstanceId]));
	output.TangentInW = normalize(mul(float4(input.Tangent, 0), World[input.InstanceId]));
	output.Tex = input.Tex;
	
	return output;
}