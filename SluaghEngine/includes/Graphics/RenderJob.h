#ifndef SE_GRAPHICS_RENDER_JOB_H_
#define SE_GRAPHICS_RENDER_JOB_H_
#include "Pipeline.h"
#include <functional>
#include <vector>
namespace SE
{
	namespace Graphics
	{
		enum class RenderGroup : uint32_t
		{
			PRE_PASS_0,
			PRE_PASS_1,
			PRE_PASS_2,
			PRE_PASS_3,
			PRE_PASS_4,
			PRE_PASS_5,
			RENDER_PASS_0,
			RENDER_PASS_1,
			RENDER_PASS_2,
			RENDER_PASS_3,
			RENDER_PASS_4,
			RENDER_PASS_5,
			POST_PASS_0,
			POST_PASS_1,
			POST_PASS_2,
			POST_PASS_3,
			POST_PASS_4,
			POST_PASS_5
		};


		struct RenderJob
		{
			Pipeline pipeline;
			unsigned int vertexCount = 0;
			unsigned int vertexOffset = 0;
			unsigned int indexCount = 0;
			unsigned int indexOffset = 0;
			unsigned int instanceCount = 0;
			unsigned int instanceOffset = 0;
			unsigned int maxInstances = 0;
			unsigned int ThreadGroupCountX = 0;
			unsigned int ThreadGroupCountY = 0;
			unsigned int ThreadGroupCountZ = 0;
			std::vector<std::function<void(int, int)>> mappingFunc;
			Utilz::GUID specialHaxxor;
		};
	}
}


#endif
