#include "OptionState.h"
#include <Profiler.h>
#include "CoreInit.h"

using namespace SE;
using namespace Gameplay;

OptionState::OptionState()
{
	StartProfile;
	fileParser.entityIndex = 0;
	fileParser.ParseFiles("OptionsMenu.HuD");
	fileParser.InitiateTextures();

	fullscreen = CoreInit::subSystems.optionsHandler->GetOptionBool("Window", "fullScreen", fullscreen);
	masterVol = CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "masterVolume", masterVol);
	effectVol = CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "effectVolume", effectVol);
	voiceVol = CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "voiceVolume", voiceVol);
	bakgroundVol = CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "bakgroundVolume", bakgroundVol);


	auto returnToMain = [this]()->void
	{
		this->CurrentState = State::MAIN_MENU_STATE;
	}; std::function<void()> back = returnToMain;

	auto setNewOptions = [this]()->void
	{
		CoreInit::subSystems.optionsHandler->SetOptionBool("Window", "fullScreen", fullscreen);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "masterVolume", masterVol);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "effectVolume", effectVol);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "voiceVolume", voiceVol);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "bakgroundVolume", bakgroundVol);
		CoreInit::subSystems.optionsHandler->Trigger();
	}; std::function<void()> newOptions = setNewOptions;

	auto setDefaultOptions = [this]()->void
	{
		CoreInit::subSystems.optionsHandler->SetOptionBool("Window", "fullScreen", false);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "masterVolume", 100);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "effectVolume", 50);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "voiceVolume", 60);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "bakgroundVolume", 20);
		CoreInit::subSystems.optionsHandler->Trigger();

	}; std::function<void()> defaultOptions = setDefaultOptions;

	auto setVVp1 = [this]()->void
	{
		if (voiceVol < 100)
			voiceVol++;
	}; std::function<void()> vVplus1 = setVVp1;
	auto setVVp10 = [this]()->void
	{
		if (voiceVol <= 90)
			voiceVol + 10;
	}; std::function<void()> vVplus10 = setVVp10;
	auto setVVn1 = [this]()->void
	{
		if (voiceVol > 0)
			voiceVol--;
	}; std::function<void()> vVminus1 = setVVn1;
	auto setVVn10 = [this]()->void
	{
		if (voiceVol >= 10)
			voiceVol - 10;
	}; std::function<void()> vVminus10 = setVVn10;
	auto setBVp1 = [this]()->void
	{
		if (bakgroundVol < 100)
			bakgroundVol++;
	}; std::function<void()> bVplus1 = setBVp1;
	auto setBVp10 = [this]()->void
	{
		if (bakgroundVol <= 90)
			bakgroundVol + 10;
	}; std::function<void()> bVplus10 = setBVp10;
	auto setBVn1 = [this]()->void
	{
		if (bakgroundVol > 0)
			bakgroundVol--;
	}; std::function<void()> bVminus1 = setBVn1;
	auto setBVn10 = [this]()->void
	{
		if (bakgroundVol >= 10)
			bakgroundVol - 10;
	}; std::function<void()> bVminus10 = setBVn10;
	auto setEVp1 = [this]()->void
	{
		if (effectVol < 100)
			effectVol++;
	}; std::function<void()> eVplus1 = setEVp1;
	auto setEVp10 = [this]()->void
	{
		if (effectVol <= 90)
			effectVol + 10;
	}; std::function<void()> eVplus10 = setEVp10;
	auto setEVn1 = [this]()->void
	{
		if (effectVol > 0)
			effectVol--;
	}; std::function<void()> eVminus1 = setEVn1;
	auto setEVn10 = [this]()->void
	{
		if (effectVol >= 10)
			effectVol - 10;
	}; std::function<void()> eVminus10 = setEVn10;
	auto setMVp1 = [this]()->void
	{
		if (masterVol < 100)
			masterVol++;
	}; std::function<void()> mVplus1 = setMVp1;
	auto setMVp10 = [this]()->void
	{
		if (masterVol <= 90)
			masterVol + 10;
	}; std::function<void()> mVplus10 = setMVp10;
	auto setMVn1 = [this]()->void
	{
		if (masterVol > 0)
			masterVol--;
	}; std::function<void()> mVminus1 = setMVn1;
	auto setMVn10 = [this]()->void
	{
		if (masterVol >= 10)
			masterVol - 10;
	}; std::function<void()> mVminus10 = setMVn10;


	for (auto& button : fileParser.ButtonVector)
	{
		if (button.rectName == "voiceVol+1")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, vVplus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "voiceVol-1")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, vVminus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "voiceVol+10")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, vVplus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "voiceVol-10")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, vVminus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backVol+1")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, bVplus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backVol-1")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, bVminus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backVol+10")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, bVplus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backVol-10")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, bVminus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "effectVol+1")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, eVplus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "effectVol-1")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, eVminus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "effectVol+10")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, eVplus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "effectVol-10")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, eVminus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVol+1")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, mVplus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVol-1")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, mVminus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVol+10")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, mVplus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVol-10")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, mVminus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "fillscreen")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, back, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "return")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, back, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "default")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, defaultOptions, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "accept")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, newOptions, button.textName, button.hoverTex, button.PressTex);
		}
	}

	fileParser.GUIButtons.DrawButtons();

	ProfileReturnVoid;
}

SE::Gameplay::OptionState::~OptionState()
{
	StartProfile;
	fileParser.GUIButtons.DeleteButtons();
	ProfileReturnVoid;
}

IGameState::State SE::Gameplay::OptionState::Update(void *& passableInfo)
{
	StartProfile;
	bool pressed = CoreInit::subSystems.window->ButtonDown(uint32_t(GameInput::ACTION));
	bool released = CoreInit::subSystems.window->ButtonUp(uint32_t(GameInput::ACTION));
	int mousePosX, mousePosY;
	CoreInit::subSystems.window->GetMousePos(mousePosX, mousePosY);


	fileParser.GUIButtons.ButtonHover(mousePosX, mousePosY, pressed, released);
	ProfileReturn(CurrentState);
}
