#ifndef _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#define _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#include <DirectXMath.h>
struct Particle {
	DirectX::XMFLOAT3 pos;
	float size;
	DirectX::XMFLOAT3 velocity;
	float opacity;
	DirectX::XMFLOAT3 startEmitPos;
	float age;
	uint32_t type;
};
#endif // !_SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
