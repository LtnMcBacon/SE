#include "PausState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;
using namespace Window;
PausState::PausState()
{
}

PausState::~PausState()
{
}





GameState::State PausState::Update(IWindow* Input,void* &passableInfo)
{
	StartProfile;

	GameState::State empty = State::PAUSE_STATE;
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


	StopProfile;
	return empty;
}