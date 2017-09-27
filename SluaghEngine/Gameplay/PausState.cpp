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

	bool running = true;
	while (running)
	{

		if (Input->ButtonPressed(0))
		{
			running = false;
		}

		Input->Frame();
	}


	ProfileReturn(empty);
}