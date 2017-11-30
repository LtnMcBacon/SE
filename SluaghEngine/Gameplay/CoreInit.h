#ifndef SE_GAMEPLAY_CORE_INIT
#define SE_GAMEPLAY_CORE_INIT

#include <Core\IEngine.h>

namespace SE
{
	namespace Gameplay
	{
		struct CoreInit
		{
			static void Init(Core::IEngine* engineIn);
			static void Shutdown();

			static SE::Core::IEngine::Subsystems subSystems;
			static SE::Core::IEngine::ManagerWrapper managers;
			static SE::Core::IEngine* engine;

		};
	}
}

#endif //SE_GAMEPLAY_CORE_INIT