Texture2D DiffuseColor : register(t0);

Texture2D NormalMap : register(t1);

Texture2D ShadowMap : register(t2);

SamplerState sampAni : register(s0);
SamplerState sampPoint : register(s1);

static const float MAP_SIZE = 1024.0f;
static const float MAP_X = 1.0f / MAP_SIZE;

struct Light
{
	float4 colour;
	float4 pos;
	uint4 castShadow;
};

cbuffer LightDataBuffer : register(b0)
{
	uint4 nrOfLights;
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

	for (int i = 0; i < nrOfLights.x; i++)
	{
		light = pointLights[i].pos.xyz - input.PosInW;
		distance = length(light);
		attenuation = max(0, 1.0f - (distance / pointLights[i].pos.w));	//Dämpning

		light /= distance;

		float normalDotLight = saturate(dot(input.NormalInW.xyz, light));
		float3 calcDiffuse = normalDotLight * pointLights[i].colour.xyz * (DiffuseColor.Sample(sampAni, input.Tex).xyz * diffuse.xyz);
		
		if(pointLights[i].castShadow.x == 1){
		
			float4 lPos = mul(input.PosInW, lViewProj);
		
			// Division by w
			lPos.xyz /= lPos.w;
			
			// From light space to texture space "[-1, 1] to [0, 1]";
			float2 ProjTexC = float2(0.5f * lPos.x + 0.5f, -0.5f * lPos.y + 0.5f);
			
			// Get pixels distance from the light and define a depth bias
			float depth = lPos.z;
			float depthBias = 0.00032f;
			
			// Texture sampling uses the texel position to look up a texel value. An offset can be applied to the position before lookup.
			// Sample shadow map to get nearest depth to the light and add 
			
			float s0 = (ShadowMap.Sample(sampPoint, ProjTexC.xy).r + depthBias < depth) ? 0.25f : 1.0f;
			float s1 = (ShadowMap.Sample(sampPoint, ProjTexC.xy + float2(MAP_X, 0.0f)).r + depthBias < depth) ? 0.25f : 1.0f;
			float s2 = (ShadowMap.Sample(sampPoint, ProjTexC.xy + float2(0.0f, MAP_X)).r + depthBias < depth) ? 0.25f : 1.0f;
			float s3 = (ShadowMap.Sample(sampPoint, ProjTexC.xy + float2(MAP_X, MAP_X)).r + depthBias < depth) ? 0.25f : 1.0f;
			
			// Transform to texel space
			float2 texelPos = MAP_SIZE * ProjTexC.xy;
			
			// Determine the interpolation amount
			float2 t = frac(texelPos);
			
			// Interpolate results
			shadowFactor = lerp(lerp(s0, s1, t.x), lerp(s2, s3, t.x), t.y);
		
		}
		
		//Calculate specular term
		float3 V = cameraPos.xyz - input.PosInW.xyz;
		float3 H = normalize(light + V);
		float3 power = pow(saturate(dot(input.NormalInW.xyz, H)), specPower);
		float3 colour = pointLights[i].colour.xyz * specular.xyz;
		float3 specularTot = power * colour * normalDotLight;


		totLight = ((calcDiffuse + specularTot) * attenuation) * shadowFactor + totLight;
	}
	
	return float4(totLight, 1.0f);
}