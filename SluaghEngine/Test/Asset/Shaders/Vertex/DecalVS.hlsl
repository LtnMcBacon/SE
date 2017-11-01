cbuffer DecalsWorld : register(b0)
{
	float4x4 gWorld[256];
};

cbuffer OncePerFrame : register(b1)
{
	float4x4 gViewProj;
};

struct VS_OUT
{
	float4 posNDC : SV_POSITION;
	float4 posWVP : POSITION;
	uint instanceID : INSTANCEID;
};

struct VS_IN
{
	uint vertexID : SV_VertexID;
	uint instanceID : SV_InstanceID;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;
	float3 positions[14] = {
		float3(-0.5f, -0.5f, -0.5f),
		float3(0.5f, -0.5f, -0.5f),
		float3(0.5f, -0.5f,  0.5f),
		float3(0.5f,  0.5f, -0.5f),
		float3(0.5f,  0.5f,  0.5f),
		float3(-0.5f,  0.5f,  0.5f),
		float3(0.5f, -0.5f,  0.5f),
		float3(-0.5f, -0.5f,  0.5f),
		float3(-0.5f, -0.5f, -0.5f),
		float3(-0.5f,  0.5f,  0.5f),
		float3(-0.5f,  0.5f, -0.5f),
		float3(0.5f,  0.5f, -0.5f),
		float3(-0.5f, -0.5f, -0.5f),
		float3(0.5f, -0.5f, -0.5f)
	};
	//SV_Position also does some z-divide or whatever in the hardware thats not done just by the matrix mul. That's why we also have the posWVP
	output.posNDC = mul(mul(float4(positions[input.vertexID],1.0f), gWorld[input.instanceID]), gViewProj);
	output.posWVP = output.posNDC;
	output.instanceID = input.instanceID;
	return output;
	
}