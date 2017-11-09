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
	float speed;
	float emitRate;
	float lifeTime;
	float tangentialValue;
	float radialValue;
	float gravityValue;
	float pSize;
	int circular;
	int gravityCheck;
	int emit;

};
cbuffer ParticleTransform : register(b2)
{
	float4x4 World;
};
struct ParticleInfo {
	float3 pos : POSITION;
	float pad : PAD1;
	float3 velocity : VELOCITY;
	float pad2 : PAD2;
	float3 startEmitPos : ORIGIN;
	float pad5 : PAD5;
	float size : SIZE;
	float pad4 : PAD4;
	float opacity : OPACITY;
	float age : AGE;
	uint type : TYPE;
};

[maxvertexcount(2)]
void GS_main(point ParticleInfo input[1], inout PointStream<ParticleInfo> ptStream)
{
	input[0].age += 1 / 60.0f;
	
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
			particle.opacity = 0.0f;
			particle.age = 0.0f;
			particle.type = 1;
			particle.pad = 0;
			particle.pad2 = 0;
			particle.pad4 = 0;
			particle.pad5 = 0;
			ptStream.Append(particle);
			ptStream.RestartStrip();
			input[0].age = 0.0f;
		}
		ptStream.Append(input[0]);
		ptStream.RestartStrip();
	}
	else
	{
		if (input[0].age <= lifeTime)
		{
			float3 radialVector = (input[0].pos - input[0].startEmitPos) * 1;
			normalize(radialVector);
			radialVector.z = 0;
			float tanX = radialVector.x;
			float3 tanVector = radialVector;
			tanVector.x = -tanVector.y;
			tanVector.y = tanX;
			
			
			if (input[0].age < 0.5)
			{
				input[0].opacity += 1/60.0f;
				
			}
			else
				input[0].opacity = 1 - input[0].age/ lifeTime;
			if(circular)
			{
				if (input[0].pos.x != emitPos.x || input[0].pos.y != emitPos.y)
					input[0].pos += (input[0].velocity + radialVector * radialValue + tanVector * tangentialValue) * speed;
				else
					input[0].pos += input[0].velocity * speed;
			}
			else
			{
				input[0].pos += input[0].velocity * speed;
			}
			if (gravityCheck)
			{
				input[0].pos += gravity * gravityValue * speed;
			}	
			ptStream.Append(input[0]);
			ptStream.RestartStrip();
		}
	}
	
}