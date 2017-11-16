cbuffer DecalsWorld : register(b0)
{
    float4x4 gWorld[256];
};

cbuffer OncePerFrame : register(b1)
{
    float4x4 View;
    float4x4 ViewProj;
};


struct VS_OUT
{
    float4 posNDC : SV_POSITION;
    float4 posWVP : POSITION;
    uint instanceID : INSTANCEID;
};

float planeWidth = 50;
float planeHeight = 2;

float3 positions[6] =
{
    float3(-planeWidth, planeHeight, planeWidth),
		float3(planeWidth, planeHeight, planeWidth),
		float3(planeWidth, planeHeight, -planeWidth),
		float3(-planeWidth, planeHeight, planeWidth),
		float3(planeWidth, planeHeight, -planeWidth),
		float3(-planeWidth, planeHeight, -planeWidth)
};


float2 uvs[6] =
{
    float2(),
		float2(),
		float2(),
		float2(),
		float2(),
		float2()
};

VS_OUT VS_main(uint VertexID : SV_VertexID)
{


    VS_OUT output;
	//SV_Position also does some z-divide or whatever in the hardware thats not done just by the matrix mul. That's why we also have the posWVP
    output.posNDC = mul(mul(float4(positions[input.vertexID], 1.0f), gWorld[input.instanceID]), ViewProj);
    output.posWVP = output.posNDC;
    output.instanceID = input.instanceID;
    return output;
	
}
