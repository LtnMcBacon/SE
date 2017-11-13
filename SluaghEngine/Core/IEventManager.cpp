#include <IEventManager.h>
#include "EventManager.h"

DECLDIR_CORE SE::Core::IEventManager * SE::Core::CreateEventManager(const IEventManager::InitializationInfo& info)
{
	return new EventManager(info);
}
