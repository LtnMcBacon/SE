#include"PlayState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

PlayState::PlayState()
{

}

PlayState::PlayState(Window::IWindow* Input)
{
	this->Input = Input;
}

PlayState::~PlayState()
{

}

IGameState::State PlayState::Update(void* passableInfo)
{
	StartProfile;

	IGameState::State empty = State::PLAY_STATE;

	bool running = true;
	while (running)
	{
		
		/*Code*/
	}

	ProfileReturn(empty);
}