#include <IRenderableManager.h>
#include "RenderableManager.h"
DECLDIR_CORE SE::Core::IRenderableManager * SE::Core::CreateRenderableManager(const IRenderableManager::InitializationInfo& initInfo)
{
	return new RenderableManager(initInfo);
}
