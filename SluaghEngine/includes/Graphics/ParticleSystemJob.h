#ifndef SE_GRAPHICS_PARTICLE_SYSTEM_JOB_H_
#define SE_GRAPHICS_PARTICLE_SYSTEM_JOB_H_
#include <DirectXMath.h>

namespace SE
{
	namespace Graphics
	{
		struct ParticleSystemJob
		{
			int vertexShaderHandle;
			int geometryShaderHandle;
			int pixelShaderHandle;
			int vertexBufferHandle;
			DirectX::XMFLOAT4X4 transform;
			int vertexCount;
		};
	}
}


#endif
