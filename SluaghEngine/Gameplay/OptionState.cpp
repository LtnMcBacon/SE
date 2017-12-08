#include "OptionState.h"
#include <Profiler.h>
#include "CoreInit.h"

using namespace SE;
using namespace Gameplay;

static DirectX::XMFLOAT4 colour = DirectX::XMFLOAT4(0.0, 0.5, 0.5, 1.0);

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
	wasFullscreen = fullscreen;

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
		fullscreen = false;
		masterVol = 100;
		effectVol = 50;
		voiceVol = 60;
		bakgroundVol = 20;
		CoreInit::managers.textManager->SetText(buttonEnt["fullScreenBtn"], L"AV");
		CoreInit::managers.textManager->SetText(buttonEnt["masterVolumeBtn"], std::to_wstring(static_cast<int>(masterVol)));
		CoreInit::managers.textManager->SetText(buttonEnt["EffectVolumeBtn"], std::to_wstring(static_cast<int>(effectVol)));
		CoreInit::managers.textManager->SetText(buttonEnt["voiceVolumeBtn"], std::to_wstring(static_cast<int>(voiceVol)));
		CoreInit::managers.textManager->SetText(buttonEnt["backgroundVolumeBtn"], std::to_wstring(static_cast<int>(bakgroundVol)));
		CoreInit::subSystems.optionsHandler->SetOptionBool("Window", "fullScreen", fullscreen);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "masterVolume", masterVol);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "effectVolume", effectVol);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "voiceVolume", voiceVol);
		CoreInit::subSystems.optionsHandler->SetOptionUnsignedInt("Audio", "bakgroundVolume", bakgroundVol);
		CoreInit::subSystems.optionsHandler->Trigger();

	}; std::function<void()> defaultOptions = setDefaultOptions;

	auto setVVp1 = [this]()->void
	{
		if (voiceVol < 100)
		{
			voiceVol++;
			CoreInit::managers.textManager->SetText(buttonEnt["voiceVolumeBtn"], std::to_wstring(static_cast<int>(voiceVol)));
		}
	}; std::function<void()> vVplus1 = setVVp1;
	auto setVVp10 = [this]()->void
	{
		if (voiceVol <= 90)
		{
			voiceVol += 10;
			CoreInit::managers.textManager->SetText(buttonEnt["voiceVolumeBtn"], std::to_wstring(static_cast<int>(voiceVol)));
		}
	}; std::function<void()> vVplus10 = setVVp10;
	auto setVVn1 = [this]()->void
	{
		if (voiceVol > 0)
		{
			voiceVol--;
			CoreInit::managers.textManager->SetText(buttonEnt["voiceVolumeBtn"], std::to_wstring(static_cast<int>(voiceVol)));
		}
	}; std::function<void()> vVminus1 = setVVn1;
	auto setVVn10 = [this]()->void
	{
		if (voiceVol >= 10)
		{
			voiceVol -= 10;
			CoreInit::managers.textManager->SetText(buttonEnt["voiceVolumeBtn"], std::to_wstring(static_cast<int>(voiceVol)));
		}
	}; std::function<void()> vVminus10 = setVVn10;
	auto setBVp1 = [this]()->void
	{
		if (bakgroundVol < 100)
		{
			bakgroundVol++;
			CoreInit::managers.textManager->SetText(buttonEnt["backgroundVolumeBtn"], std::to_wstring(static_cast<int>(bakgroundVol)));
		}
	}; std::function<void()> bVplus1 = setBVp1;
	auto setBVp10 = [this]()->void
	{
		if (bakgroundVol <= 90)
		{
			bakgroundVol += 10;
			CoreInit::managers.textManager->SetText(buttonEnt["backgroundVolumeBtn"], std::to_wstring(static_cast<int>(bakgroundVol)));
		}
	}; std::function<void()> bVplus10 = setBVp10;
	auto setBVn1 = [this]()->void
	{
		if (bakgroundVol > 0)
		{
			bakgroundVol--;
			CoreInit::managers.textManager->SetText(buttonEnt["backgroundVolumeBtn"], std::to_wstring(static_cast<int>(bakgroundVol)));
		}
	}; std::function<void()> bVminus1 = setBVn1;
	auto setBVn10 = [this]()->void
	{
		if (bakgroundVol >= 10)
		{
			bakgroundVol -= 10;
			CoreInit::managers.textManager->SetText(buttonEnt["backgroundVolumeBtn"], std::to_wstring(static_cast<int>(bakgroundVol)));
		}
	}; std::function<void()> bVminus10 = setBVn10;
	auto setEVp1 = [this]()->void
	{
		if (effectVol < 100)
		{
			effectVol++;
			CoreInit::managers.textManager->SetText(buttonEnt["EffectVolumeBtn"], std::to_wstring(static_cast<int>(effectVol)));
		}
	}; std::function<void()> eVplus1 = setEVp1;
	auto setEVp10 = [this]()->void
	{
		if (effectVol <= 90)
		{
			effectVol += 10;
			CoreInit::managers.textManager->SetText(buttonEnt["EffectVolumeBtn"], std::to_wstring(static_cast<int>(effectVol)));
		}
	}; std::function<void()> eVplus10 = setEVp10;
	auto setEVn1 = [this]()->void
	{
		if (effectVol > 0)
		{
			effectVol--;
			CoreInit::managers.textManager->SetText(buttonEnt["EffectVolumeBtn"], std::to_wstring(static_cast<int>(effectVol)));
		}
	}; std::function<void()> eVminus1 = setEVn1;
	auto setEVn10 = [this]()->void
	{
		if (effectVol >= 10)
		{
			effectVol -= 10;
			CoreInit::managers.textManager->SetText(buttonEnt["EffectVolumeBtn"], std::to_wstring(static_cast<int>(effectVol)));
		}
	}; std::function<void()> eVminus10 = setEVn10;
	auto setMVp1 = [this]()->void
	{
		if (masterVol < 100)
		{
			masterVol++;
			CoreInit::managers.textManager->SetText(buttonEnt["masterVolumeBtn"], std::to_wstring(static_cast<int>(masterVol)));
		}
	}; std::function<void()> mVplus1 = setMVp1;
	auto setMVp10 = [this]()->void
	{
		if (masterVol <= 90)
		{
			masterVol += 10;
			CoreInit::managers.textManager->SetText(buttonEnt["masterVolumeBtn"], std::to_wstring(static_cast<int>(masterVol)));
		}
	}; std::function<void()> mVplus10 = setMVp10;
	auto setMVn1 = [this]()->void
	{
		if (masterVol > 0)
		{
			masterVol--;
			CoreInit::managers.textManager->SetText(buttonEnt["masterVolumeBtn"], std::to_wstring(static_cast<int>(masterVol)));
		}
	}; std::function<void()> mVminus1 = setMVn1;
	auto setMVn10 = [this]()->void
	{
		if (masterVol >= 10)
		{
			masterVol -= 10;
			CoreInit::managers.textManager->SetText(buttonEnt["masterVolumeBtn"], std::to_wstring(static_cast<int>(masterVol)));
		}
	}; std::function<void()> mVminus10 = setMVn10;

	auto setFull = [this]()->void
	{
		if (fullscreen == false)
		{
			CoreInit::managers.textManager->SetText(buttonEnt["fullScreenBtn"], L"PÅ");
			fullscreen = true;
		}
		else
		{
			CoreInit::managers.textManager->SetText(buttonEnt["fullScreenBtn"], L"AV");
			fullscreen = false;
		}
	}; std::function<void()> setFullScreen = setFull;

	auto doNothing = [this]()->void
	{
		
	}; std::function<void()> nothing = doNothing;

	for (auto& button : fileParser.ButtonVector)
	{
		button.PositionY += 20;
		if (button.rectName == "voiceVolumeBtnUpOne")
		{
			buttonEnt["voiceVolumeBtnUpOne"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY +15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"+1";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["voiceVolumeBtnUpOne"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["voiceVolumeBtnUpOne"], true);

			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, vVplus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "voiceVolumeBtnDownOne")
		{
			buttonEnt["voiceVolumeBtnDownOne"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"-1";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["voiceVolumeBtnDownOne"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["voiceVolumeBtnDownOne"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 15, button.Width, button.Height, button.layerDepth, button.rectName, vVminus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "voiceVolumeBtnUpTen")
		{
			buttonEnt["voiceVolumeBtnUpTen"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"+10";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["voiceVolumeBtnUpTen"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["voiceVolumeBtnUpTen"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, vVplus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "voiceVolumeBtnDownTen")
		{
			buttonEnt["voiceVolumeBtnDownTen"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"-10";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["voiceVolumeBtnDownTen"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["voiceVolumeBtnDownTen"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 15, button.Width, button.Height, button.layerDepth, button.rectName, vVminus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backGroundVolumeBtnUpOne")
		{
			buttonEnt["backGroundVolumeBtnUpOne"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"+1";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["backGroundVolumeBtnUpOne"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["backGroundVolumeBtnUpOne"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, bVplus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backGroundVolumeBtnDownOne")
		{
			buttonEnt["backGroundVolumeBtnDownOne"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"-1";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["backGroundVolumeBtnDownOne"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["backGroundVolumeBtnDownOne"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 15, button.Width, button.Height, button.layerDepth, button.rectName, bVminus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backGroundVolumeBtnUpTen")
		{
			buttonEnt["backGroundVolumeBtnUpTen"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"+10";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["backGroundVolumeBtnUpTen"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["backGroundVolumeBtnUpTen"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, bVplus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backGroundVolumeBtnDownTen")
		{
			buttonEnt["backGroundVolumeBtnDownTen"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"-10";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["backGroundVolumeBtnDownTen"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["backGroundVolumeBtnDownTen"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 15, button.Width, button.Height, button.layerDepth, button.rectName, bVminus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "EffectVolumeBtnUpOne")
		{
			buttonEnt["EffectVolumeBtnUpOne"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"+1";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["EffectVolumeBtnUpOne"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["EffectVolumeBtnUpOne"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, eVplus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "EffectVolumeBtnDownOne")
		{
			buttonEnt["EffectVolumeBtnDownOne"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"-1";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["EffectVolumeBtnDownOne"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["EffectVolumeBtnDownOne"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 15, button.Width, button.Height, button.layerDepth, button.rectName, eVminus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "EffectVolumeBtnUpTen")
		{
			buttonEnt["EffectVolumeBtnUpTen"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"+10";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["EffectVolumeBtnUpTen"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["EffectVolumeBtnUpTen"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, eVplus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "EffectVolumeBtnDownTen")
		{
			buttonEnt["EffectVolumeBtnDownTen"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"-10";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["EffectVolumeBtnDownTen"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["EffectVolumeBtnDownTen"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 15, button.Width, button.Height, button.layerDepth, button.rectName, eVminus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVolumeBtnUpOne")
		{
			buttonEnt["masterVolumeBtnUpOne"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"+1";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["masterVolumeBtnUpOne"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["masterVolumeBtnUpOne"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, mVplus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVolumeBtnDownOne")
		{
			buttonEnt["masterVolumeBtnDownOne"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"-1";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["masterVolumeBtnDownOne"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["masterVolumeBtnDownOne"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 15, button.Width, button.Height, button.layerDepth, button.rectName, mVminus1, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVolumeBtnUpTen")
		{
			buttonEnt["masterVolumeBtnUpTen"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"+10";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["masterVolumeBtnUpTen"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["masterVolumeBtnUpTen"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, mVplus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVolumeBtnDownTen")
		{
			buttonEnt["masterVolumeBtnDownTen"] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX;
			cInfo.info.posY = button.PositionY + 15;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"-10";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["masterVolumeBtnDownTen"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["masterVolumeBtnDownTen"], true);
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 15, button.Width, button.Height, button.layerDepth, button.rectName, mVminus10, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "BackBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, back, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "DefaultBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, defaultOptions, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "AcceptBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, newOptions, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "voiceVolumeBtn")
		{	
			boxNames[0] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Knights.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 30;
			cInfo.info.posY = button.PositionY - 38;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"RÖSTVOLYM";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.6, 0.6);

			CoreInit::managers.textManager->Create(boxNames[0], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(boxNames[0], true);

			buttonEnt["voiceVolumeBtn"] = CoreInit::managers.entityManager->Create();
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 10;
			cInfo.info.posY = button.PositionY + 5;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = std::to_wstring(static_cast<int>(voiceVol));
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);
			
			CoreInit::managers.textManager->Create(buttonEnt["voiceVolumeBtn"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["voiceVolumeBtn"], true);

			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, nothing, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "backgroundVolumeBtn")
		{
			boxNames[1] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Knights.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 30;
			cInfo.info.posY = button.PositionY - 38;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"MUSIKVOLYM";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.6, 0.6);

			CoreInit::managers.textManager->Create(boxNames[1], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(boxNames[1], true);

			buttonEnt["backgroundVolumeBtn"] = CoreInit::managers.entityManager->Create();
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 10;
			cInfo.info.posY = button.PositionY + 5;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = std::to_wstring(static_cast<int>(bakgroundVol));
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["backgroundVolumeBtn"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["backgroundVolumeBtn"], true);

			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, nothing, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "EffectVolumeBtn")
		{
			boxNames[2] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Knights.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 30;
			cInfo.info.posY = button.PositionY - 38;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"EFFEKTVOLYM";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.6, 0.6);

			CoreInit::managers.textManager->Create(boxNames[2], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(boxNames[2], true);

			buttonEnt["EffectVolumeBtn"] = CoreInit::managers.entityManager->Create();
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 10;
			cInfo.info.posY = button.PositionY + 5;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = std::to_wstring(static_cast<int>(effectVol));
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["EffectVolumeBtn"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["EffectVolumeBtn"], true);

			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, nothing, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "masterVolumeBtn")
		{
			boxNames[3] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Knights.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 30;
			cInfo.info.posY = button.PositionY - 38;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"MÄSTARVOLYM";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.6, 0.6);

			CoreInit::managers.textManager->Create(boxNames[3], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(boxNames[3], true);

			buttonEnt["masterVolumeBtn"] = CoreInit::managers.entityManager->Create();
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 10;
			cInfo.info.posY = button.PositionY + 5;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = std::to_wstring(static_cast<int>(masterVol));
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["masterVolumeBtn"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["masterVolumeBtn"], true);

			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, nothing, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "fullScreenBtn")
		{
			boxNames[4] = CoreInit::managers.entityManager->Create();
			SE::Core::ITextManager::CreateInfo cInfo;
			cInfo.font = "Knights.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 30;
			cInfo.info.posY = button.PositionY - 33;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			cInfo.info.text = L"FULLSKÄRM";
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.6, 0.6);

			CoreInit::managers.textManager->Create(boxNames[4], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(boxNames[4], true);

			buttonEnt["fullScreenBtn"] = CoreInit::managers.entityManager->Create();
			cInfo.font = "Ancient.spritefont";
			cInfo.info.colour = colour;
			cInfo.info.effect = Graphics::Effect::NoEffect;
			cInfo.info.posX = button.PositionX + 10;
			cInfo.info.posY = button.PositionY + 10;
			cInfo.info.width = button.Width;
			cInfo.info.height = button.Height;
			cInfo.info.layerDepth = 0;
			if (fullscreen == false)
			{
				cInfo.info.text = L"AV";
			}
			else
			{
				cInfo.info.text = L"PÅ";
			}
			cInfo.info.hashString = std::hash<std::wstring>()(cInfo.info.text);
			cInfo.info.anchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.screenAnchor = DirectX::XMFLOAT2(0, 0);
			cInfo.info.rotation = 0;
			cInfo.info.scale = DirectX::XMFLOAT2(0.8, 0.8);

			CoreInit::managers.textManager->Create(buttonEnt["fullScreenBtn"], cInfo);
			CoreInit::managers.textManager->ToggleRenderableText(buttonEnt["fullScreenBtn"], true);

			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY + 5, button.Width, button.Height, button.layerDepth, button.rectName, setFullScreen, button.textName, button.hoverTex, button.PressTex);
		}
	}

	
	fileParser.GUIButtons.DrawButtons();

	ProfileReturnVoid;
}

SE::Gameplay::OptionState::~OptionState()
{
	StartProfile;
	fileParser.GUIButtons.DeleteButtons();
	for (auto& entity : buttonEnt)
	{
		CoreInit::managers.textManager->ToggleRenderableText(entity.second, false);
		CoreInit::managers.entityManager->Destroy(entity.second);
	}
	for (int i = 0; i < 5;i++)
	{
		CoreInit::managers.textManager->ToggleRenderableText(boxNames[i], false);
		CoreInit::managers.entityManager->Destroy(boxNames[i]);
	}
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


	bool isFullscreen = false;
	isFullscreen = CoreInit::subSystems.optionsHandler->GetOptionBool("Window", "fullScreen", isFullscreen);

	if (isFullscreen!= wasFullscreen)
	{
		fileParser.GUIButtons.reloadButtons();
		wasFullscreen = isFullscreen;
	}
	ProfileReturn(CurrentState);
}
