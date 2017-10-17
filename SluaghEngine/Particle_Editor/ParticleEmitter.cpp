#include "ParticleEmitter.h"



void ParticleEmitter::setVelocity(float x, float y)
{
	for (size_t i = 0; i < particleCount; i++)
	{
		particlePool[i].velocity = DirectX::XMFLOAT2(x, y);
	}
}

ParticleEmitter::ParticleEmitter()
{
	particleCount = 1;
	
}


ParticleEmitter::~ParticleEmitter()
{

}
