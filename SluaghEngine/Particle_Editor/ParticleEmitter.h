#ifndef _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#define _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#include <DirectXMath.h>
class ParticleEmitter
{
public:
	struct Particle {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT2 size;
		DirectX::XMFLOAT3 color;
//		float opacity;
		float age;
		uint32_t type;
	};
	int particleCount;
	
	Particle particlePool[1000];

	/*
	* @brief Sets the velocity on a particle
	* @param[in] x Variable for the x velocity
	* @param[in] y Variable for the y velocity
	*/


	ParticleEmitter();
	~ParticleEmitter();
};

#endif // !_SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
