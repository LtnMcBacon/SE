#include "PauseState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

PauseState::PauseState()
{
	

}

PauseState::PauseState(Window::IWindow* Input)
{
	StartProfile;
	this->input = Input;
	fileParser.ParseFiles("PauseMenu.HuD");
	fileParser.InitiateTextures();
	//fileParser.SetFunctionOnPress("resumeBtn"; /*func*/ );
	//fileParser.SetFunctionOnPress("optionsBtn"; /*func*/ );
	////fileParser.SetFunctionOnPress("quitBtn"; /*func*/ );
	ProfileReturnVoid;
}

PauseState::~PauseState()
{

}





IGameState::State PauseState::Update(void* &passableInfo)
{
	StartProfile;

	bool pressed = input->ButtonDown(uint32_t(GameInput::ACTION));
	int mousePosX, mousePosY;
	input->GetMousePos(mousePosX, mousePosY);
	


	IGameState::State empty = State::PAUSE_STATE;
	//*(int*)passableInfo +=1;


	if (input->ButtonPressed(0))
	{
		empty = State::MAIN_MENU_STATE;
	}


	ProfileReturn(empty);
}