#define P_EMITTER 0
#define P_PARTICLE 1
cbuffer OncePerFrame : register(b1)
{
	float4x4 View;
	float4x4 ViewProj;
};


cbuffer CameraPos : register(b3)
{
	float3 camUpVector;
	float padCam;
	float3 eyePos;
	float padCam2;
};

struct GS_IN {
	float3 pos : POSITION;
	float size : SIZE;
	float3 velocity : VELOCITY;
	float opacity : OPACITY;
	float3 startEmitPos : ORIGIN;
	float age : AGE;
	uint type : TYPE;
};
struct GS_OUT {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float opacity : OPACITY;
};

[maxvertexcount(4)]
void GS_main(point GS_IN input[1], inout TriangleStream< GS_OUT > streamOutput)
{
	GS_OUT output = (GS_OUT)0;
	float4 p[4];


	
	if(input[0].type == P_PARTICLE)
	{
		float3 pNormal = eyePos - input[0].pos;
		float3 rightVec = cross(pNormal, camUpVector);
		rightVec = normalize(rightVec);
		float2 UVs[4];
	
		p[0] = float4(input[0].pos - (rightVec * input[0].size) - (camUpVector * input[0].size), 1.0f);
		UVs[0] = float2(1.0f, 0.0f);
		p[1] = float4(input[0].pos - (rightVec * input[0].size) + (camUpVector * input[0].size), 1.0f);
		UVs[1] = float2(0.0f, 0.0f);
		p[2] = float4(input[0].pos + (rightVec * input[0].size) - (camUpVector * input[0].size), 1.0f);
		UVs[2] = float2(1.0f , 1.0f);
		p[3] = float4(input[0].pos + (rightVec * input[0].size) + (camUpVector * input[0].size), 1.0f);
		UVs[3] = float2(0.0f, 1.0f);
		
		for (int i = 0; i < 4; i++)
		{
			
			output.pos = mul(p[i], ViewProj);
			output.tex = UVs[i];
			output.opacity = input[0].opacity;
		//	output.bloom = input[0].bloom;
			streamOutput.Append(output);
		}
		streamOutput.RestartStrip();
	}
}