Texture2D DiffuseColor : register(t0);
Texture2D NormalMap : register(t1);
TextureCube ShadowMap : register(t2);

SamplerState sampAni : register(s0);

struct Light
{
	float4 colour;
	float4 pos;
	uint4 castShadow;
};

cbuffer LightDataBuffer : register(b2)
{
	uint4 nrOfLights;
	Light pointLights[20];
};

cbuffer CameraPos : register(b3)
{
	float3 camUpVector;
	float pad;
	float3 eyePos;
	float pad2;
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
	float3 normalTs = NormalMap.Sample(sampAni, input.Tex).xyz;
	normalTs = normalize(normalTs * 2.0f - 1.0f);
	float3x3 Tbm = float3x3(normalize(input.TangentInW).xyz, normalize(input.BinormalInW).xyz, normalize(input.NormalInW).xyz);
	float3 normalWorld = mul(normalTs, Tbm);
	
	float shadowFactor = 1.0f;
	float distance;
	float specPower = specular.w;
	float4 textureColor = DiffuseColor.Sample(sampAni, input.Tex);
	
	float3 diffuseContribution = float3(0.0f, 0.0f, 0.0f);
	float3 specularContribution = float3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < nrOfLights.x; i++)
	{
		light = pointLights[i].pos.xyz - input.PosInW;
		distance = length(light);
		float divby = (distance / pointLights[i].pos.w) + 1.0f;
		attenuation = (1.0f / (divby * divby)) - 0.25f;
	
		light /= distance;
	
		float normalDotLight = saturate(dot(normalWorld.xyz, light));
	
	
		if (i == nrOfLights.y)
		{
			float3 sampVec = normalize(input.PosInW - pointLights[i].pos.xyz);
			float mapDepth = ShadowMap.Sample(sampAni, sampVec).r;
			if (mapDepth + 0.002f < distance / pointLights[i].pos.w)
				shadowFactor = 0.25f;
		}
	
		//Calculate specular term
		float3 V = eyePos.xyz - input.PosInW.xyz;
		float3 H = normalize(light + V);
		float3 power = pow(saturate(dot(input.NormalInW.xyz, H)), specPower);
		float3 colour = pointLights[i].colour.xyz * specular.xyz;
		specularContribution += power * colour * normalDotLight * shadowFactor;
		diffuseContribution += normalDotLight * pointLights[i].colour.xyz * shadowFactor;
	}
	
	return saturate(float4(textureColor.rgb * (attenuation * (diffuseContribution + specularContribution) + float3(0.1f, 0.1f, 0.1f)), textureColor.a));
}