
cbuffer DecalsInverseViewProj : register(b0)
{
	float4x4 gInvViewProj;
};

cbuffer DecalsInverseWorld : register(b1)
{
	float4x4 gInvWorld[256];
};



SamplerState gAnisotropicSam : register(s0);

struct VS_OUT
{
	float4 posNDC : SV_POSITION;
	float4 posWVP : POSITION;
	uint instanceID : INSTANCEID;
};

Texture2D DepthBuffer : register(t0);
Texture2D DecalDiffuse : register(t1);


float4 PS_main(VS_OUT input) : SV_Target
{
//	return float4(1.0f,1.0f,1.0f,1.0f);
	//Get UVs to sample depth
	input.posWVP.xyz /= input.posWVP.w;
	float depth = DepthBuffer.Load(uint3(input.posNDC.xy, 0)).r;
	
	//Get world pos by multiplying with invViewproj
	float4 worldPos = mul(float4(input.posWVP.x, input.posWVP.y, depth, 1.0f), gInvViewProj);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1.0f;


	//Transform worldPos into Decals local space
	float4 localPosition = mul(worldPos, gInvWorld[input.instanceID]);
	//return float4(localPosition.x + 0.5f, 1.0f - (localPosition.y + 0.5f), 0.0f, 1.0f);
	clip(0.5f - abs(localPosition.xyz)); //If it is outside the box's local space we do nothing
	float2 decalUV = localPosition.xy + 0.5f;
	decalUV.y = 1.0f - decalUV.y;

	
//	return float4(1.0f,1.0f,1.0f,1.0f);
	float4 decalColor = DecalDiffuse.Sample(gAnisotropicSam, decalUV);
	clip(decalColor.a - 0.05f); //If the decal is very transparent right here, we dont need to do any blending.
	return decalColor;
}