Texture2D DiffuseColor : register(t0);

SamplerState sampAni : register(s0);

struct Light
{
	float4 colour;
	float4 pos;
	float4 castShadow;
};

cbuffer LightDataBuffer : register(b2)
{
	uint4 nrOfLights;
	Light pointLights[20];
};

cbuffer CameraPos : register(b3)
{
	float4 cameraPos;
};

cbuffer MaterialAttributes : register(b4)
{
	float4 diffuse;
	float4 ambient;
	float4 specular;
};

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float4 PosInW : WORLDPOS;
	float4 NormalInW : NORMALINW;
	float4 BinormalInW : BINORMALINW;
	float4 TangentInW : TANGENTINW;
	float2 Tex : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_TARGET
{
	float attenuation = 1.0f;
	float3 totLight = ambient.xyz;
	float3 light = float3(0.0, 0.0, 0.0);
	float distance;
	float specPower = specular.w;

	for (int i = 0; i < nrOfLights.x; i++)
	{
		light = pointLights[i].pos.xyz - input.PosInW;
		distance = length(light);
		attenuation = max(0, 1.0f - (distance / pointLights[i].pos.w));	//Dämpning

		light /= distance;

		float normalDotLight = saturate(dot(input.NormalInW.xyz, light));
		float3 calcDiffuse = normalDotLight * pointLights[i].colour.xyz * (DiffuseColor.Sample(sampAni, input.Tex).xyz * diffuse.xyz);
		
		//Calculate specular term
		float3 V = cameraPos.xyz - input.PosInW.xyz;
		float3 H = normalize(light + V);
		float3 power = pow(saturate(dot(input.NormalInW.xyz, H)), specPower);
		float3 colour = pointLights[i].colour.xyz * specular.xyz;
		float3 specularTot = power * colour * normalDotLight;


		totLight = ((calcDiffuse + specularTot) * attenuation) + totLight;
	}
	
	return float4(totLight, 1.0f);
}