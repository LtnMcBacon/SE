#include "CharacterCreationState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;
using namespace Window;

CharacterCreationState::CharacterCreationState()
{

}
CharacterCreationState::~CharacterCreationState()
{

}
GameState::State CharacterCreationState::Update(IWindow* Input, void* &passableInfo)
{
	StartProfile;

	GameState::State empty;
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

	StopProfile;
	return empty;
}