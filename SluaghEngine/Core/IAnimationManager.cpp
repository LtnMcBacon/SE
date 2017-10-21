#include <IAnimationManager.h>
#include "AnimationManager.h"

DECLDIR_CORE SE::Core::IAnimationManager * SE::Core::CreateAnimationManager(const IAnimationManager::InitializationInfo & initInfo)
{
	return new AnimationManager(initInfo);
}
