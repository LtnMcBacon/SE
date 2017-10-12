#include "GameOverState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;


GameOverState::GameOverState()
{
}

GameOverState::GameOverState(Window::IWindow * Input)
{
	this->input = Input;
}

GameOverState::~GameOverState()
{
}

IGameState::State GameOverState::Update(void* &passableInfo)
{
	StartProfile;
	IGameState::State empty = State::GAME_OVER_STATE;

	
	if (input->ButtonPressed(0))
	{
			
		empty = State::MAIN_MENU_STATE;
	}
		
	/*Code*/

	

	ProfileReturn(empty);
}