#include <Core\Engine.h>
#include <Gameplay\Game.h>
#include <Windows.h>

#ifdef _DEBUG
#pragma comment(lib, "CoreD.lib")
#else
#pragma comment(lib, "Core.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "GameplayD.lib")
#else
#pragma comment(lib, "Gameplay.lib")
#endif

using namespace SE;
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	auto& engine = Core::Engine::GetInstance();
	Core::Engine::InitializationInfo initInfo;
	engine.Init(initInfo);

	SE::Gameplay::Game game;
	game.Initiate();
	game.Run();
	// game.Shutdown();

	engine.Release();

	return 0;
}