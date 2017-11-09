cbuffer CameraViewProject : register(b0)
{
	float4 CameraPos;
};

struct GS_IN
{
	float4 Pos : SV_POSITION;
	float4 PosInW : WORLDPOS;
	float4 NormalInW : NORMALINW;
	float2 Tex : TEXCOORD;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float4 PosInW : WORLDPOS;
	float4 NormalInW : NORMALINW;
	float2 Tex : TEXCOORD;
};


[maxvertexcount(3)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > streamOutput)
{
	GS_OUT output = (GS_OUT)0;
	
	float3 Norm = normalize(input[0].NormalInW.xyz + input[1].NormalInW.xyz + input[2].NormalInW.xyz);
	float3 VnPosCam = float3(input[0].PosInW.x - CameraPos.x, input[0].PosInW.y - CameraPos.y, input[0].PosInW.z - CameraPos.z);
	float dotNP = dot(VnPosCam, Norm);

	if (dotNP <= 0)
	{
		for (int i = 0; i < 3; i++)
		{
			output.Pos = input[i].Pos;
			output.PosInW = input[i].PosInW;
			output.NormalInW = float4(Norm, 1.0f);
			output.Tex = input[i].Tex;
			streamOutput.Append(output);
		}
		streamOutput.RestartStrip();
	}
}