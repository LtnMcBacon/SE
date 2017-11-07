#include "GameStateTest.h"
#include<Gameplay\IGameState.h>
#include <Profiler.h>
#include <Core\IEngine.h>
#include <chrono>
#include <window/IWindow.h>
#include <Gameplay\MainMenuState.h>
#include <Gameplay\PauseState.h>
#include "Gameplay/PlayState.h"
#include "Gameplay/GameOverState.h"
#include "Gameplay/CharacterCreationState.h"
#include <Gameplay\Game.h>
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
bool GameStateTest::Run(SE::DevConsole::IConsole* console)
{

	StartProfile;
	auto engine = Core::CreateEngine();
	Game game;
	engine->Init();
	game.Initiate(engine);
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();

	

	bool running = true;

	IGameState::State SwitchState = IGameState::MAIN_MENU_STATE;


	IGameState::State OldState = SwitchState;

	subSystem.window->MapActionButton(0, Window::Key1);
	subSystem.window->MapActionButton(1, Window::KeyW);
	subSystem.window->MapActionButton(2, Window::KeyS);
	subSystem.window->MapActionButton(3, Window::KeyA);
	subSystem.window->MapActionButton(4, Window::KeyD);

	

	IGameState* Game = new PauseState(subSystem.window);
	

	
	subSystem.window->MapActionButton(Exit, Window::KeyEscape);
	void* passableInfo = nullptr;
	while (running)
	{
		//if (subSystem.window->ButtonPressed(Exit))
			//running = false;
		
		SwitchState = Game->Update(passableInfo);
		if (SwitchState != OldState)
		{

			switch (SwitchState)
			{
			case IGameState::MAIN_MENU_STATE:
				console->Print("Making Main Menu State!\n");
				delete Game;
				Game = new MainMenuState(subSystem.window);
				//std::cout << "passableInfo: " << *(int*)passableInfo << std::endl;
	
				break;
			case IGameState::PLAY_STATE:
				console->Print("Making Play State!\n");
				break;
			case IGameState::GAME_OVER_STATE:
				console->Print("Making Game over State!\n");
				delete Game;
				Game = new GameOverState(subSystem.window);
				break;
			case IGameState::CHARACTER_CREATION_STATE:
				console->Print("Making Character Creation State!\n");
				delete Game;
				Game = new CharacterCreationState(subSystem.window);
		
				break;
			case IGameState::PAUSE_STATE:
				console->Print("Making Pause State!\n");
				delete Game;
				Game = new PauseState(subSystem.window);
				//std::cout << "passableInfo: " << *(int*)passableInfo << std::endl;
		
				break;
			default:
				break;
			}
			
		}
		//delete passableInfo;
		OldState = SwitchState;
		engine->BeginFrame();
		engine->EndFrame();
	}
	delete Game;

	game.Shutdown();
	engine->Release(); delete engine;

	ProfileReturnConst(true);
}
