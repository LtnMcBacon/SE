#include <ILightManager.h>
#include "LightManager.h"

DECLDIR_CORE SE::Core::ILightManager * SE::Core::CreateLightManager(const ILightManager::InitializationInfo & initInfo)
{
	return new LightManager(initInfo);
}
