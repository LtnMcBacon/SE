#define P_EMITTER 0
#define P_PARTICLE 1
cbuffer velocityBuffer : register(b0)
{
	float3 velocityVec;
	float pad;
	float3 emitPos;
	float pad1;
	float3 emitRange;
	float pad5;
	float3 gravity;
	float pad4;
	float3 startPos;
	float pad6;
	float3 endPos;
	float pad7;
	float speed;
	float emitRate;
	float lifeTime;
	float tangentialValue;
	float radialValue;
	float gravityValue;
	float pSize;
	float dt;
	int circular;
	int gravityCheck;
	int emit;
	int particlePath;
	int bloomCheck;
};
cbuffer lockBuffer : register(b1)
{
	bool locked;
}
cbuffer ParticleTransform : register(b2)
{
	float4x4 World;
};
cbuffer CamBuffer : register(b3)
{
	float3 camUpVector;
	float padCam;
	float3 eyePos;
	float padCam2;
};
struct ParticleInfo {
	float3 pos : POSITION;
	float size : SIZE;
	float3 velocity : VELOCITY;
	float opacity : OPACITY;
	float3 startEmitPos : ORIGIN;
	float age : AGE;
	uint type : TYPE;
};
bool equal(float3 start, float3 end)
{
	
	if((abs(start.x - end.x) < 0.005) && (abs(start.y - end.y) < 0.005) && (abs(start.z - end.z) < 0.005))
		return true;
	else
		return false;
}
[maxvertexcount(2)]
void GS_main(point ParticleInfo input[1], inout PointStream<ParticleInfo> ptStream)
{
	input[0].age += dt;	
	if	(input[0].type == P_EMITTER)
	{
		input[0].pos = emitPos;	
		input[0].pos = mul(float4(input[0].pos, 1.0f), World).xyz;
		if (input[0].age > emitRate && emit == 1)
		{
			ParticleInfo particle;
			particle.pos = input[0].pos + emitRange;
			particle.startEmitPos = input[0].pos;
			particle.velocity = float3(velocityVec.x, velocityVec.y, velocityVec.z);
			particle.size = pSize;
			particle.opacity = 1.0f;
			particle.age = 0.0f;
			particle.type = 1;
			//particle.bloom = bloomCheck;
			ptStream.Append(particle);
			ptStream.RestartStrip();
			input[0].age = 0.0f;
		}
		ptStream.Append(input[0]);
		ptStream.RestartStrip();
	}
	else
	{
		float3 startPosition = input[0].startEmitPos;
		if (input[0].age <= lifeTime && !particlePath)
		{
			float3 radialVector = (input[0].startEmitPos - input[0].pos);
			radialVector = normalize(radialVector);
			float3 lookAt = eyePos - input[0].startEmitPos;
			lookAt = normalize(lookAt);
			float3 tanVector = cross(radialVector, lookAt);
			tanVector = normalize(tanVector);
		
			input[0].opacity = 1 - input[0].age/ lifeTime;
			if(circular == 1)
			{
				if (!equal(input[0].pos, emitPos))
					input[0].pos += ((radialVector * radialValue) + (tanVector * tangentialValue)) * dt;
				else
					input[0].pos += input[0].velocity * dt* speed;
			}
			else
			{
				input[0].pos += input[0].velocity * dt * speed;
			}
			if (gravityCheck)
			{
				input[0].pos += gravity * gravityValue * dt * speed;
			}	
			ptStream.Append(input[0]);
			ptStream.RestartStrip();
		}
		else if (particlePath == 1)
		{
			if(!equal(input[0].pos, endPos))
			{
				if(locked)
				{
					float3 finalDirection = endPos - input[0].pos;
					input[0].velocity = finalDirection;
				}
				else	
				{
					input[0].velocity = float3(0, 0, 0);
				}
	
				input[0].opacity = 1 - input[0].age/ lifeTime;
	
				input[0].pos += input[0].velocity * dt * speed;
			
				ptStream.Append(input[0]);
				ptStream.RestartStrip();
			}

		}
	}
}