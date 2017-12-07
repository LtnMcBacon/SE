#ifndef _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#define _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#include <DirectXMath.h>
struct Particle {
	DirectX::XMFLOAT3 pos;
	float pad;
	DirectX::XMFLOAT3 velocity;
	float pad2;
	DirectX::XMFLOAT3 startEmitPos;
	float pad5;
	float size;
	float pad4;
	float opacity;
	float age;
	uint32_t type;
	uint32_t bloom;
};
#endif // !_SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
