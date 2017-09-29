#ifndef SE_GRAPHICS_LINE_RENDER_JOB_H_
#define SE_GRAPHICS_LINE_RENDER_JOB_H_
#include <DirectXMath.h>

namespace SE
{
	namespace Graphics
	{
		struct LineRenderJob
		{
			int vertexBufferHandle;
			int vertexShaderHandle;
			int pixelShaderHandle;
			uint32_t firstVertex;
			uint32_t verticesToDrawCount;
			DirectX::XMFLOAT4X4 transform;
		};
	}
}


#endif
