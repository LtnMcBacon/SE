#include <IEventManager.h>
#include "EventManager.h"

DECLDIR_CORE SE::Core::IEventManager * SE::Core::CreateEventManager()
{
	return new EventManager;
}
