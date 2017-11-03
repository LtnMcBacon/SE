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
	this->input = Input;
	
	fileParser.ParseFiles("MainMenu.HuD");

	Core::IGUIManager::CreateInfo GuiManager;

	for (auto& HUDElement: fileParser.MainMenuElmVec)
	{


		auto entity = CoreInit::managers.entityManager->Create();

		GuiManager.entity = entity;
		GuiManager.texFile = HUDElement.textName;
		GuiManager.texInfo.pos.x = HUDElement.PositionX;
		GuiManager.texInfo.pos.y = HUDElement.PositionY;
		GuiManager.texInfo.layerDepth = HUDElement.layerDepth;
		GuiManager.texInfo.scale.x = HUDElement.Width;
		GuiManager.texInfo.scale.y = HUDElement.Height;

		
		
	}
	for (auto& HUDText: fileParser.MainMenuTextVec)
	{

	}
	CoreInit::managers.guiManager->Create(GuiManager);

}

MainMenuState::~MainMenuState()
{

}

IGameState::State MainMenuState::Update(void* &passableInfo)
{
	StartProfile;


	HUDInputCheck InputChecker;
	

	for (auto & HUDElement: fileParser.MainMenuElmVec)
	{
		if (HUDElement.Btn)
		{
			InputChecker.posX = HUDElement.PositionX;
			InputChecker.posY = HUDElement.PositionY;
			InputChecker.height = HUDElement.Height;
			InputChecker.width = HUDElement.Width;
			input->GetMousePos(InputChecker.mousePosX, InputChecker.mousePosY);

			InputChecker.pressed = input->ButtonDown(uint32_t(GameInput::ACTION));

			ElementFunctionality.HoverButton(InputChecker);

		}

	}
	IGameState::State empty = State::MAIN_MENU_STATE;
			
	
	if (input->ButtonPressed(0))
	{
			
		empty = State::PAUSE_STATE;
	}


	passableInfo = new int(10);

	ProfileReturn(empty);
	
}