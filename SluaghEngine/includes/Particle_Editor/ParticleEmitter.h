#ifndef _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#define _SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
#include <DirectXMath.h>
struct Particle {
	DirectX::XMFLOAT3 pos;
	float pad;
	DirectX::XMFLOAT3 velocity;
	float pad2;
	DirectX::XMFLOAT3 color;
	float pad3;
	float size;
	float pad4;
	float opacity;
	float age;
	uint32_t type;
};
#endif // !_SE_PARTICLE_EDITOR_PARTICLE_EMITTER_H_
