#include <IDecalManager.h>
#include "DecalManager.h"

DECLDIR_CORE SE::Core::IDecalManager* SE::Core::CreateDecalManager(const IDecalManager::InitializationInfo& initInfo)
{
	return new DecalManager(initInfo);
}
