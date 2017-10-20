#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
{
	particleCount = 1000;
}
ParticleEmitter::~ParticleEmitter()
{

}
void ParticleEmitter::setVelocity(float x, float y)
{
	for (size_t i = 0; i < particleCount; i++)
	{
		particlePool[i].velocity = DirectX::XMFLOAT2(x, y);
	}
}
