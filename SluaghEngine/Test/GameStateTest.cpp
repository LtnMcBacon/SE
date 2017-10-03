#include "GameStateTest.h"
#include<Gameplay\IGameState.h>
#include <Profiler.h>
#include <Core\Engine.h>
#include <chrono>
#include <window/IWindow.h>
#include <Gameplay\MainMenuState.h>
#include <Gameplay\PauseState.h>
#include "Gameplay/PlayState.h"
#include "Gameplay/GameOverState.h"
#include "Gameplay/CharacterCreationState.h"
using namespace SE;
using namespace Gameplay;
using namespace Test;


GameStateTest::GameStateTest()
{

}

GameStateTest::~GameStateTest()
{

}
enum
{
	Exit
};
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

	IGameState::State SwitchState = IGameState::MAIN_MENU_STATE;


	IGameState::State OldState = SwitchState;

	Window::IWindow* Input = e.GetWindow();
	
	Input->MapActionButton(0, Window::KeyEscape);
	Input->MapActionButton(1, Window::KeyW);
	Input->MapActionButton(2, Window::KeyS);
	Input->MapActionButton(3, Window::KeyA);
	Input->MapActionButton(4, Window::KeyD);
	IGameState* Game = new MainMenuState(Input);
	
	auto w = e.GetWindow();
	w->MapActionButton(Exit, Window::KeyEscape);
	void* passableInfo = nullptr;
	while (running)
	{
		if (w->ButtonPressed(Exit))
			running = false;
		
		SwitchState = Game->Update(passableInfo);
		if (SwitchState != OldState)
		{

			switch (SwitchState)
			{
			case IGameState::MAIN_MENU_STATE:
				console->Print("Making Main Menu State!\n");
				delete Game;
				Game = new MainMenuState(Input);
				std::cout << "passableInfo: " << *(int*)passableInfo << std::endl;
	
				break;
			case IGameState::PLAY_STATE:
				console->Print("Making Pause State!\n");
				
				break;
			case IGameState::GAME_OVER_STATE:
				console->Print("Making Game over State!\n");
				delete Game;
				Game = new GameOverState(Input);
				break;
			case IGameState::CHARACTER_CREATION_STATE:
				console->Print("Making Character Creation State!\n");
				delete Game;
				Game = new CharacterCreationState(Input);
		
				break;
			case IGameState::PAUSE_STATE:
				console->Print("Making Pause State!\n");
				delete Game;
				Game = new PauseState(Input);
				std::cout << "passableInfo: " << *(int*)passableInfo << std::endl;
		
				break;
			default:
				break;
			}
			
		}
		delete passableInfo;
		OldState = SwitchState;
		e.Frame(1/60.f);
	}
	delete Game;

	e.Release();

	ProfileReturnConst(true);
}
