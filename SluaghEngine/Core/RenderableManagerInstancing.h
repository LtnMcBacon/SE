#ifndef SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_
#define SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_
#include <Entity.h>
#include <Graphics\RenderJob.h>
namespace SE
{
	namespace Core
	{
		class RenderableManagerInstancing
		{
		public:
			RenderableManagerInstancing();
			~RenderableManagerInstancing();

			void AddEntity(const Entity& entity, const Graphics::RenderJob& job);
		};
	}
}

#endif //SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_