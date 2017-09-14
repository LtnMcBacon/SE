#include <Core\Engine.h>
#ifdef _DEBUG
#pragma comment(lib, "CoreD.lib")
#else
#pragma comment(lib, "Core.lib")
#endif

using namespace SE;
int main()
{
	auto& engine = Core::Engine::GetInstance();
	Core::Engine::InitializationInfo initInfo;
	engine.Init(initInfo);
	
	// Gameplay game;
	// game.Init();
	// game.Run();
	// game.Shutdown();

	engine.Release();

	return 0;
}