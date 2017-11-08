Texture2D DiffuseColor : register(t0);

Texture2D NormalMap : register(t1);

TextureCube ShadowMap : register(t2);

SamplerState sampAni : register(s0);
SamplerState sampPoint : register(s1);


struct Light
{
	float4 colour;
	float4 pos;
};

cbuffer LightDataBuffer : register(b0)
{
	uint4 nrOfLights; //x-component for number, y-component for which light is casting shadow.
	Light pointLights[20];
};

cbuffer CameraPos : register(b1)
{
	float4 cameraPos;
};

cbuffer MaterialAttributes : register(b2)
{
	float4 diffuse;
	float4 ambient;
	float4 specular;
};

cbuffer LightViewProj : register(b3)
{
	float4x4 lViewProj;
}

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
	float3 totLight = ambient.xyz + float3(0.1, 0.1, 0.1);
	float3 light = float3(0.0, 0.0, 0.0);
	float shadowFactor = 1.0f;
	float distance;
	float specPower = specular.w;
	float3 textureColor = DiffuseColor.Sample(sampAni, input.Tex).xyz;
	
	float3 diffuseContribution = float3(0.0f, 0.0f, 0.0f);
	float3 specularContribution = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < nrOfLights.x; i++)
	{
		light = pointLights[i].pos.xyz - input.PosInW;
		distance = length(light);
		float divby = (distance / pointLights[i].pos.w) + 1.0f;
		attenuation = 1.0f / (divby * divby);

		light /= distance;

		float normalDotLight = saturate(dot(input.NormalInW.xyz, light));
		
		
		if(i == nrOfLights.y)
		{
			float3 sampVec = normalize(input.PosInW - pointLights[i].pos.xyz);
			float mapDepth = ShadowMap.Sample(sampAni, sampVec).r;
			if(mapDepth + 0.0001f < distance / pointLights[i].pos.w)
				shadowFactor = 0.25f;
		}
		
		//Calculate specular term
		float3 V = cameraPos.xyz - input.PosInW.xyz;
		float3 H = normalize(light + V);
		float3 power = pow(saturate(dot(input.NormalInW.xyz, H)), specPower);
		float3 colour = pointLights[i].colour.xyz * specular.xyz;
		specularContribution += power * colour * normalDotLight * shadowFactor;
		diffuseContribution += normalDotLight * pointLights[i].colour.xyz * shadowFactor;
	}
	
	return saturate(float4(textureColor * (diffuseContribution + specularContribution + float3(0.1f, 0.1f, 0.1f)),1.0f));

}