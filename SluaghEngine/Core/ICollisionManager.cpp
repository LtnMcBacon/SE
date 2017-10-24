#include <ICollisionManager.h>
#include "CollisionManager.h"

DECLDIR_CORE SE::Core::ICollisionManager * SE::Core::CreateCollisionManger(const ICollisionManager::InitializationInfo & initInfo)
{
	return new CollisionManager(initInfo);
}
