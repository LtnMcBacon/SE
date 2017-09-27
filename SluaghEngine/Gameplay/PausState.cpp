#include "PauseState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

PauseState::PauseState()
{
}

PauseState::PauseState(Window::IWindow* Input)
{
	this->Input = Input;
}

PauseState::~PauseState()
{
}





IGameState::State PauseState::Update(void* &passableInfo)
{
	StartProfile;

	IGameState::State empty = State::PAUSE_STATE;
	*(int*)passableInfo +=1;

	
	

		if (Input->ButtonPressed(0))
		{
			empty = State::MAIN_MENU_STATE;
		}

		
	


	ProfileReturn(empty);
}