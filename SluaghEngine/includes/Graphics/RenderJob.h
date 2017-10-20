#ifndef SE_GRAPHICS_RENDER_JOB_H_
#define SE_GRAPHICS_RENDER_JOB_H_
#include "Pipeline.h"
#include <functional>
namespace SE
{
	namespace Graphics
	{
		struct RenderJob
		{
			Pipeline pipeline;
			int vertexCount;
			int vertexOffset;
			int indexCount;
			int indexOffset;
			int instanceCount;
			int instanceOffset;
			int maxInstances;
			std::function<void(int, int)> mappingFunc;
		};
	}
}


#endif
