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
			int vertexCount = 0;
			int vertexOffset = 0;
			int indexCount = 0;
			int indexOffset = 0;
			int instanceCount = 0;
			int instanceOffset = 0;
			int maxInstances = 0;
			std::function<void(int, int)> mappingFunc;
		};
	}
}


#endif
