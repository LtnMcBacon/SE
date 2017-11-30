#include "CoreInit.h"

#ifdef _DEBUG
#pragma comment(lib, "CoreD.lib")
#else
#pragma comment(lib, "Core.lib")
#endif

SE::Core::IEngine::Subsystems SE::Gameplay::CoreInit::subSystems = {};
SE::Core::IEngine::ManagerWrapper SE::Gameplay::CoreInit::managers = {};
SE::Core::IEngine* SE::Gameplay::CoreInit::engine = nullptr;

void SE::Gameplay::CoreInit::Init(Core::IEngine* engineIn)
{
	engine = engineIn;
	subSystems = engine->GetSubsystems();
	managers = engine->GetManagers();
}

void SE::Gameplay::CoreInit::Shutdown()
{

}
