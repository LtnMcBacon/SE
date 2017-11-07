#include "MainMenuState.h"
#include "CoreInit.h"

using namespace SE;
using namespace Gameplay;
#include <Profiler.h>
#include <Gameplay\HUD_Functionality.h>

MainMenuState::MainMenuState()
{

}

MainMenuState::MainMenuState(Window::IWindow * Input)
{
	auto PausChange = [this]()->void
	{
		this->CurrentState = State::PAUSE_STATE;
	};

	fileParser.ParseFiles("MainMenu.HuD");
	fileParser.InitiateTextures();
	std::function<void()> func = PausChange;
	fileParser.SetFunctionOnPress("Button", func);
	this->input = Input;
	
}

MainMenuState::~MainMenuState()
{

}

IGameState::State MainMenuState::Update(void* &passableInfo)
{
	StartProfile;

	bool pressed = input->ButtonDown(uint32_t(GameInput::ACTION));
	int mousePosX, mousePosY;
	input->GetMousePos(mousePosX, mousePosY);
	fileParser.checkPressed(pressed,mousePosX,mousePosY);

	

	
	IGameState::State empty = State::MAIN_MENU_STATE;
			
	
	if (input->ButtonPressed(0))
	{
			
		CurrentState = State::PAUSE_STATE;
	}


	passableInfo = new int(10);

	ProfileReturn(CurrentState);
	
}