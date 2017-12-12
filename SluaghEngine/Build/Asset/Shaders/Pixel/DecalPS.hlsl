
cbuffer DecalsInverseViewProj : register(b0)
{
	float4x4 gInvViewProj;
};

cbuffer DecalsInverseWorld : register(b1)
{
	float4x4 gInvWorld[256];
};

struct DecalInfo
{
	float opacity;
	float ambiance;
	float2 padding;
};

cbuffer DecalOpacity : register(b2)
{
	DecalInfo decalInfos[256];
};

struct Light
{
	float4 colour;
	float4 pos;
};

cbuffer LightDataBuffer : register(b3)
{
	uint4 nrOfLights; //x-component for number, y-component for which light is casting shadow.
	Light pointLights[64];
};

SamplerState gAnisotropicSam : register(s0);

struct VS_OUT
{
	float4 posNDC : SV_POSITION;
	float4 posWVP : POSITION;
	uint instanceID : INSTANCEID;
};

Texture2D DepthBuffer : register(t0);
Texture2D DecalDiffuse : register(t1);
TextureCube ShadowMap : register(t2);


float4 PS_main(VS_OUT input) : SV_Target
{
//	return float4(1.0f,1.0f,1.0f,1.0f);
	//Get UVs to sample depth
	input.posWVP.xyz /= input.posWVP.w;
	float depth = DepthBuffer.Load(uint3(input.posNDC.xy, 0)).r;
	
	//Get world pos by multiplying with invViewproj
	float4 worldPos = mul(float4(input.posWVP.x, input.posWVP.y, depth, 1.0f), gInvViewProj);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1.0f;


	//Transform worldPos into Decals local space
	float4 localPosition = mul(worldPos, gInvWorld[input.instanceID]);
	//return float4(localPosition.x + 0.5f, 1.0f - (localPosition.y + 0.5f), 0.0f, 1.0f);
	clip(0.5f - abs(localPosition.xyz)); //If it is outside the box's local space we do nothing
	float2 decalUV = localPosition.xy + 0.5f;
	decalUV.y = 1.0f - decalUV.y;

	float4 decalColor = DecalDiffuse.Sample(gAnisotropicSam, decalUV);
	float opac = decalInfos[input.instanceID].opacity;
	float ambiance = decalInfos[input.instanceID].ambiance;
	decalColor.a *= opac;
	clip(decalColor.a - 0.05f); //If the decal is very transparent right here, we dont need to do any blending.
	

	float3 normal = -normalize(cross(ddy(worldPos), ddx(worldPos))).xyz;
	float attenuation = 1.0f;
		
	float3 diffuseContribution = float3(0.0f, 0.0f, 0.0f);
	if(decalInfos[input.instanceID].ambiance < 1.0f)
	{
		for (int i = 0; i < nrOfLights.x; i++)
		{
			float shadowFactor = 1.0f;
			float3 light = pointLights[i].pos.xyz - worldPos.xyz;
			float distance = length(light);
			if(distance < pointLights[i].pos.w)
			{
				
				light /= distance;
				float normalDotLight = saturate(dot(normal, light));
				if(normalDotLight > 0.0f)
				{
					float divby = (distance / pointLights[i].pos.w) + 1.0f;
					float intensity = pointLights[i].colour.w;
					attenuation = (intensity / (divby * divby)) - intensity / 4;
	
					if (i == nrOfLights.y)
					{
						float3 sampVec = normalize(worldPos.xyz - pointLights[i].pos.xyz);
						float mapDepth = ShadowMap.Sample(gAnisotropicSam, sampVec).r;
						float ndist = distance / pointLights[i].pos.w;
						if (mapDepth + 0.002f < distance / pointLights[i].pos.w)
							shadowFactor = ndist;
					}
					diffuseContribution += attenuation * normalDotLight * pointLights[i].colour.xyz * shadowFactor;
				}
			}
		}
	}
	
	return saturate(float4(decalColor.xyz * (diffuseContribution + decalInfos[input.instanceID].ambiance.xxx), decalColor.a));

}