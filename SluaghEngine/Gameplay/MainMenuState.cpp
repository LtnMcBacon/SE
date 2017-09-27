#include "MainMenuState.h"
using namespace SE;
using namespace Gameplay;
#include <Profiler.h>

MainMenuState::MainMenuState()
{

}

MainMenuState::MainMenuState(Window::IWindow * Input)
{
	this->Input = Input;
}

MainMenuState::~MainMenuState()
{

}

IGameState::State MainMenuState::Update( void* &passableInfo)
{
	StartProfile;

	IGameState::State empty = State::MAIN_MENU_STATE;
	

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
	
	ProfileReturn(empty);
	
}