
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

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "WindowD.lib")
#else
#pragma comment(lib, "Window.lib")
#endif


using namespace SE;
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	SE::Gameplay::Game game;
	auto engine = Core::CreateEngine();
	Window::InitializationInfo initInfo;
	initInfo.winState = Window::WindowState::Record;
	initInfo.file = "Recordings/RecordingThursday Thu Nov  9 161020 2017.bin";
	Core::IEngine::InitializationInfo engineInit;
	engineInit.subSystems.window = Window::CreateNewWindow();
	engineInit.subSystems.window->Initialize(initInfo);
	auto result= engine->Init(engineInit);
	if (result < 0)
		return -1;
	game.Initiate(engine);
	game.Run();

	game.Shutdown();
	engine->Release();
	delete engine;

	return 0;
}