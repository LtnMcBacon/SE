#include "CharacterCreationState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;


CharacterCreationState::CharacterCreationState()
{

}
CharacterCreationState::CharacterCreationState(Window::IWindow * Input)
{
	this->Input = Input;
}
CharacterCreationState::~CharacterCreationState()
{

}
IGameState::State CharacterCreationState::Update(void* &passableInfo)
{
	StartProfile;

	IGameState::State empty = State::CHARACTER_CREATION_STATE;

	
	if (Input->ButtonPressed(0))
	{
			
		empty = State::MAIN_MENU_STATE;
	}
		

		/*Code*/

	

	ProfileReturn(empty);
}