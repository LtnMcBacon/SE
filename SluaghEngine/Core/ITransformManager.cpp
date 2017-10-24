#include <ITransformManager.h>
#include "TransformManager.h"

DECLDIR_CORE SE::Core::ITransformManager * SE::Core::CreateTransformManager(const SE::Core::ITransformManager::InitializationInfo& initInfo)
{
	return new TransformManager(initInfo);
}
