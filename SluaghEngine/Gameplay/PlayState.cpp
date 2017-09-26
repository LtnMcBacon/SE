#include"PlayState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

PlayState::PlayState()
{

}

PlayState::~PlayState()
{

}

GameState::State PlayState::Update(void* passableInfo)
{
	StartProfile;

	GameState::State empty = State::PLAY_STATE;

	bool running = true;
	while (running)
	{
		
		/*Code*/
	}

	StopProfile;
	return empty;
}