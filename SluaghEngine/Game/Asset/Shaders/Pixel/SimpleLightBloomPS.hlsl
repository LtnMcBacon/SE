Texture2D DiffuseColor : register(t0);

Texture2D NormalMap : register(t1);

SamplerState sampAni : register(s0);

struct Light
{
	float4 colour;
	float4 pos;
};

cbuffer LightDataBuffer : register(b2)
{
	float4 nrOfLights;
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
	float2 Tex : TEXCOORD;
};

struct PS_OUT
{
	float4 backBuffer: SV_TARGET0;
	float4 bloomBuffer: SV_TARGET1;
};

PS_OUT PS_main(PS_IN input) 
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
		attenuation = max(0, 1.0f - (distance / pointLights[i].pos.w));	//D�mpning

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
	
	
	PS_OUT output;
	output.backBuffer = float4(totLight, 1.0f);
	output.bloomBuffer = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (output.backBuffer.r > .8) output.bloomBuffer.r = output.backBuffer.r * output.backBuffer.r;
	if (output.backBuffer.g > .8) output.bloomBuffer.g = output.backBuffer.g * output.backBuffer.g;
	if (output.backBuffer.b > .8) output.bloomBuffer.b = output.backBuffer.b * output.backBuffer.b;

	return output;
}