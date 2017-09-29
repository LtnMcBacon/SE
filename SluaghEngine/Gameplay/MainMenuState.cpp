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

IGameState::State MainMenuState::Update(void* &passableInfo)
{
	StartProfile;

	IGameState::State empty = State::MAIN_MENU_STATE;
			
	
	if (Input->ButtonPressed(0))
	{
			
		empty = State::PAUSE_STATE;
	}


	passableInfo = new int(10);
	
	ProfileReturn(empty);
	
}