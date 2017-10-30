#ifndef SE_GRAPHICS_RENDER_JOB_H_
#define SE_GRAPHICS_RENDER_JOB_H_
#include "Pipeline.h"
#include <functional>
#include <vector>
namespace SE
{
	namespace Graphics
	{
		enum class RenderGroup : uint8_t
		{
			PRE_PASS_0,
			PRE_PASS_1,
			PRE_PASS_2,
			PRE_PASS_3,
			RENDER_PASS_0,
			RENDER_PASS_1,
			RENDER_PASS_2,
			RENDER_PASS_3,
			POST_PASS_0,
			POST_PASS_1,
			POST_PASS_2,
			POST_PASS_3
		};


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
			std::vector<std::function<void(int, int)>> mappingFunc;
			Utilz::GUID specialHaxxor;
		};
	}
}


#endif
