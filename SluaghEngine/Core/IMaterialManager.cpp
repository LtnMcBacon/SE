#include <IMaterialManager.h>
#include "MaterialManager.h"

DECLDIR_CORE SE::Core::IMaterialManager * SE::Core::CreateMaterialManager(const IMaterialManager::InitializationInfo & initInfo)
{
	return new MaterialManager(initInfo);
}
