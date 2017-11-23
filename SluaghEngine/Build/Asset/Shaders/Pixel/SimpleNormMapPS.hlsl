Texture2D DiffuseColor : register(t0);
Texture2D NormalMap : register(t1);
TextureCube ShadowMap : register(t2);

SamplerState sampAni : register(s0);

struct Light
{
	float4 colour;
	float4 pos;
};

cbuffer LightDataBuffer : register(b2)
{
	uint4 nrOfLights;
	Light pointLights[64];
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

cbuffer BloomProperties : register(b5)
{
	float BLOOM_BASE_MULTIPLIER;
	float BLOOM_FADE_EXPONENT;
	float BLOOM_ADDITIVE_COLOR_STRENGTH_MULTIPLIER;
	float BLOOM_AT;
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

struct PS_OUT
{
	float4 backBuffer: SV_TARGET0;
	float4 bloomBuffer: SV_TARGET1;
};

PS_OUT PS_main(PS_IN input) : SV_TARGET
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
		if(distance < pointLights[i].pos.w)
		{
			
			light /= distance;
			float normalDotLight = saturate(dot(normalWorld.xyz, light));
			if(normalDotLight > 0.0f)
			{
				float divby = (distance / pointLights[i].pos.w) + 1.0f;
				attenuation = (1.0f / (divby * divby)) - 0.25f;
			
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
				diffuseContribution += attenuation * normalDotLight * pointLights[i].colour.xyz * shadowFactor;
			}
		}
	}
	
	PS_OUT output;
	output.backBuffer = saturate(float4(textureColor.rgb * ((diffuseContribution + specularContribution) + float3(0.1f, 0.1f, 0.1f)), textureColor.a));
	output.bloomBuffer = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (output.backBuffer.r > BLOOM_AT) output.bloomBuffer.r = output.backBuffer.r * output.backBuffer.r;
	if (output.backBuffer.g > BLOOM_AT) output.bloomBuffer.g = output.backBuffer.g * output.backBuffer.g;
	if (output.backBuffer.b > BLOOM_AT) output.bloomBuffer.b = output.backBuffer.b * output.backBuffer.b;

	return output;
}