#include <Core\Engine.h>

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