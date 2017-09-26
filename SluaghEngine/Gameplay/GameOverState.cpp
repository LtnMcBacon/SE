#include "GameOverState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;


GameOverState::GameOverState()
{
}

GameOverState::GameOverState(Window::IWindow * Input)
{
	this->Input = Input;
}

GameOverState::~GameOverState()
{
}

IGameState::State GameOverState::Update(void* &passableInfo)
{
	StartProfile;
	IGameState::State empty;

	bool running = true;
	while (running)
	{
		if (Input->ButtonPressed(0))
		{
			running = false;
			empty = State::MAIN_MENU_STATE;
		}
		/*Code*/

		Input->Frame();
	}

	ProfileReturn(empty);
}