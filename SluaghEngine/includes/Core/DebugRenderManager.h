#ifndef SE_CORE_DEBUG_RENDER_MANAGER_H_
#define SE_CORE_DEBUG_RENDER_MANAGER_H_

#include "EntityManager.h"
#include "TransformManager.h"
#include <Graphics/IRenderer.h>

namespace SE
{
	namespace Core
	{
		class DebugRenderManager
		{
		public:
			DebugRenderManager( Graphics::IRenderer* renderer, const EntityManager& entityManager, TransformManager* transformManager);
			~DebugRenderManager();
			DebugRenderManager(const DebugRenderManager& other) = delete;
			DebugRenderManager(const DebugRenderManager&& other) = delete;
			DebugRenderManager& operator=(const DebugRenderManager& other) = delete;


		private:
			const EntityManager& entityManager;
			TransformManager* transformManager;
			
			Graphics::IRenderer* renderer;

		};
	}
}




#endif
