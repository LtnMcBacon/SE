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

	this->CurrentState = PAUSE_STATE;
	
	auto quitGame = [this]()->void
	{
		this->CurrentState = QUIT_GAME;
	};

	std::function<void()> shutDown = quitGame;

	auto resumeGame = [this]()->void
	{
		this->CurrentState = PLAY_STATE;
	};

	std::function<void()> resume = resumeGame;

	auto returnToMainMenu = [this]()->void
	{
		this->CurrentState = MAIN_MENU_STATE;
	};

	std::function<void()> retToMenu = returnToMainMenu;


	fileParser.GUIButtons.CreateButton(540, 50, 200, 80, 1000, "Paus", NULL, false, "NULL", "Paus.png", "Paus.png", "Paus.png");
	fileParser.GUIButtons.CreateButton(540, 250, 200, 80, 1000, "ResumeButton", resume, false, "NULL", "SpelaVidare.png", "SpelaVidare1.png", "SpelaVidare.png");
	fileParser.GUIButtons.CreateButton(540, 450, 200, 80, 1000, "ShutdownButton", shutDown, false, "NULL", "Avsluta.png", "Avsluta1.png", "Avsluta.png");
	fileParser.GUIButtons.CreateButton(540, 550, 200, 80, 1000, "ShutdownButton", retToMenu, false, "NULL", "Avsluta.png", "Avsluta1.png", "Avsluta.png");
	fileParser.GUIButtons.CreateButton(0, 0, 1280, 720, 999, "BackGround", NULL, false, "NULL", "bakgrund.png", "bakgrund.png", "bakgrund.png");
	fileParser.GUIButtons.DrawButtons();


	ProfileReturnVoid;
}

PauseState::~PauseState()
{
	StartProfile;
	fileParser.GUIButtons.DeleteButtons();
	ProfileReturnVoid;
}





IGameState::State PauseState::Update(void* &passableInfo)
{
	StartProfile;


	bool pressed = input->ButtonDown(uint32_t(GameInput::ACTION));
	bool released = input->ButtonUp(uint32_t(GameInput::ACTION));
	int mousePosX, mousePosY;
	input->GetMousePos(mousePosX, mousePosY);
	fileParser.GUIButtons.ButtonHover(mousePosX, mousePosY, pressed, released);


	ProfileReturn(CurrentState);
}