#ifndef SE_CORE_EVENT_MANAGER_H_
#define SE_CORE_EVENT_MANAGER_H_
#include <IEventManager.h>

namespace SE
{
	namespace Core
	{
		class EventManager : public IEventManager
		{
		public:
			EventManager();
			~EventManager();
		};
	}
}

#endif