#include "MainMenuState.h"
using namespace SE;
using namespace Gameplay;
using namespace Window;
#include <Profiler.h>
MainMenuState::MainMenuState()
{

}

MainMenuState::~MainMenuState()
{

}

GameState::State MainMenuState::Update(IWindow* Input, void* &passableInfo)
{
	StartProfile;

	GameState::State empty = State::MAIN_MENU_STATE;
	

	bool running = true;
	while (running)
	{

		if (Input->ButtonPressed(0))
		{
			running = false;
			empty = State::PAUSE_STATE;
		}

		Input->Frame();
	}
	int a = 10;

	passableInfo = new int(a);
	
	StopProfile;
	return empty;
	
}