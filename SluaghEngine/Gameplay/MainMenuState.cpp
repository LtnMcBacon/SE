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

		

		GuiManager.textureInfo.width = HUDElement.Width;
		GuiManager.textureInfo.height = HUDElement.Height;
		GuiManager.textureInfo.posX = HUDElement.PositionX;
		GuiManager.textureInfo.posY = HUDElement.PositionY;
		GuiManager.textureInfo.layerDepth = HUDElement.layerDepth;

		GuiManager.texture = HUDElement.textName;
		
		CoreInit::managers.guiManager->Create(entity, GuiManager);

		CoreInit::managers.guiManager->ToggleRenderableTexture(entity, true);
		

		

		
		
	}
	for (auto& HUDText: fileParser.MainMenuTextVec)
	{

	}
	

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