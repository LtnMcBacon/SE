#ifndef _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#define _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#include <DirectXMath.h>
class ParticleEmitter
{
public:

	struct Particle {
		DirectX::XMFLOAT3 pos[4];
		DirectX::XMFLOAT2 velocity;
		float lifeTime;
	};
	int particleCount;
	Particle particlePool[1000];

	/*
	* @brief Sets the velocity on a particle
	* @param[in] x Variable for the x velocity
	* @param[in] y Variable for the y velocity
	*/
	void setVelocity(float x, float y);

	ParticleEmitter();
	~ParticleEmitter();
};

#endif // !_SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
