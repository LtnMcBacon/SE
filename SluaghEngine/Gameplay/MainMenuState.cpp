#include "MainMenuState.h"
#include "CoreInit.h"

using namespace SE;
using namespace Gameplay;
#include <Profiler.h>
#include <Gameplay\HUD_Functionality.h>

MainMenuState::MainMenuState()
{

}

MainMenuState::MainMenuState(Window::IWindow * Input, std::function<void()> shutDown)
{
	StartProfile;
	auto PausChange = [this]()->void
	{
		this->CurrentState = State::PAUSE_STATE;
	};

	auto startGame = [this]()->void
	{
		this->CurrentState = State::CHARACTER_CREATION_STATE;
	};

	auto quitGame = []()->bool
	{
		
	};

	auto options = [this]() ->void
	{
		this->CurrentState = State::OPTION_STATE;
	};

	std::function<void()> func = PausChange;
	std::function<void()> begin = startGame;
	std::function<void()> option = options;

	

	fileParser.ParseFiles("MainMenu.HuD");
	fileParser.InitiateTextures();
	
	for (auto& button : fileParser.ButtonVector)
	{
		if (button.rectName == "startGameBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, begin, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "quitBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, shutDown, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "optionsBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, option, button.textName, button.hoverTex, button.PressTex);
		}
		
	}
	fileParser.GUIButtons.DrawButtons();

	this->input = Input;
	ProfileReturnVoid;
	
}

MainMenuState::MainMenuState(Window::IWindow * Input)
{
	StartProfile;
	auto PausChange = [this]()->void
	{
		this->CurrentState = State::PAUSE_STATE;
	};

	auto startGame = [this]()->void
	{
		this->CurrentState = State::CHARACTER_CREATION_STATE;
	};
	auto startTutorial = [this]()->void
	{
		this->CurrentState = State::TUTORIAL_STATE;
	};
	auto quitGame = [this]()->void
	{
		this->CurrentState = State::QUIT_GAME;
	};

	auto options = [this]() ->void
	{
		this->CurrentState = State::OPTION_STATE;
	};

	std::function<void()> func = PausChange;
	std::function<void()> begin = startGame;
	std::function<void()> shutDown = quitGame;
	std::function<void()> option = options;


	bool perhaps = false;
	bool fullscreen = CoreInit::subSystems.optionsHandler->GetOptionBool("Window", "fullScreen", perhaps);

	fileParser.ParseFiles("MainMenu.HuD");
	fileParser.InitiateTextures();

	

	for (auto& button : fileParser.ButtonVector)
	{
		if (button.rectName == "startGameBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, 1, button.rectName, begin, button.textName, button.hoverTex, button.PressTex);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 200, button.Width, button.Height, button.layerDepth, button.rectName, startTutorial, button.textName, button.hoverTex, button.PressTex);
			
		}
		else if (button.rectName == "quitBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, 1, button.rectName, shutDown, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "optionsBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, option, button.textName, button.hoverTex, button.PressTex);
		}

	}
	fileParser.GUIButtons.DrawButtons();

	this->input = Input;

	ProfileReturnVoid;
}

MainMenuState::~MainMenuState()
{
	StartProfile;
	fileParser.GUIButtons.DeleteButtons();
	ProfileReturnVoid;
}

IGameState::State MainMenuState::Update(void* &passableInfo)
{
	StartProfile;

	bool pressed = input->ButtonDown(uint32_t(GameInput::ACTION));
	bool released = input->ButtonUp(uint32_t(GameInput::ACTION));
	int mousePosX, mousePosY;
	input->GetMousePos(mousePosX, mousePosY);
	fileParser.GUIButtons.ButtonHover(mousePosX, mousePosY, pressed, released);

	IGameState::State empty = State::MAIN_MENU_STATE;

	if (input->ButtonPressed(0))
	{
			
		CurrentState = State::PAUSE_STATE;
	}

	ProfileReturn(CurrentState);	
}


