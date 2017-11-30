#include <IDebugRenderManager.h>
#include "DebugRenderManager.h"
DECLDIR_CORE SE::Core::IDebugRenderManager * SE::Core::CreateDebugRenderManager(const IDebugRenderManager::InitializationInfo & initInfo)
{
	return new DebugRenderManager(initInfo);
}
