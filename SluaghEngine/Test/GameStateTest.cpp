#include "GameStateTest.h"
#include<Gameplay\IGameState.h>
#include <Profiler.h>
#include <Core\Engine.h>
#include <chrono>
#include <window/WindowSDL.h>
#include <window/IWindow.h>
#include <Gameplay\MainMenuState.h>
#include <Gameplay\PauseState.h>
using namespace SE;
using namespace Gameplay;
using namespace Test;


GameStateTest::GameStateTest()
{

}

GameStateTest::~GameStateTest()
{

}

bool GameStateTest::Run(SE::Utilz::IConsoleBackend* console)
{

	StartProfile;
	auto& e = Core::Engine::GetInstance();
	auto& info = Core::Engine::InitializationInfo();
	auto re = e.Init(info);
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		ProfileReturnConst(false);
	}

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();

	

	bool running = true;

	void* passableInfo = nullptr;
	
	IGameState::State SwitchState = IGameState::MAIN_MENU_STATE;

	Window::IWindow* Input = e.GetWindow();
	
	Input->MapActionButton(0, Window::KeyEscape);
	
	IGameState* Game = new MainMenuState;
	

	while (running)
	{
		switch (SwitchState)
		{
		case IGameState::MAIN_MENU_STATE:
			console->Print("Entering Main menu state!\n");
			Game = new MainMenuState;
			SwitchState = Game->Update(passableInfo);
			std::cout << "passableInfo: " << *(int*)passableInfo << std::endl;
			break;
		case IGameState::PLAY_STATE:
			break;
		case IGameState::GAME_OVER_STATE:
			break;
		case IGameState::CHARACTER_CREATION_STATE:
			break;
		case IGameState::PAUSE_STATE:
			console->Print("Entering Pause state!\n");
			Game = new PauseState;
			SwitchState = Game->Update(passableInfo);
			std::cout << "passableInfo: " << *(int*)passableInfo << std::endl;
			break;
		default:
			break;
		}
		
		

		e.Frame();

	}
	delete[] passableInfo;
	StopProfile;
}