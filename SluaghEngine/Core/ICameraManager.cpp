#include <ICameraManager.h>
#include "CameraManager.h"

DECLDIR_CORE SE::Core::ICameraManager * SE::Core::CreateCameraManager(const ICameraManager::InitializationInfo & initInfo)
{
	return new CameraManager(initInfo);
}
