#include <IGUIManager.h>
#include "GUIManager.h"

DECLDIR_CORE SE::Core::IGUIManager * SE::Core::CreateGUIManager(const IGUIManager::InitializationInfo & initInfo)
{
	return new GUIManager(initInfo);
}
