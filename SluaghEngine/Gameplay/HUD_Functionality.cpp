#include "HUD_Functionality.h"
#include <HUD_Parsing.h>
#include "CoreInit.h"

using namespace std;
namespace SE
{
	namespace Gameplay
	{
		HUDButtons::HUDButtons()
		{
		}

		HUDButtons::~HUDButtons()
		{
		}

		void HUDButtons::CreateButton(int posX, int posY, int width, int height, int layerDepth, string name, std::function<void()> func, string textName, string buttonText)
		{
			ButtonElement tempElement;
			tempElement.PositionX = posX;
			tempElement.PositionY = posY;

			tempElement.Width = width;
			tempElement.Height = height;

			tempElement.layerDepth = layerDepth;

			tempElement.rectName = name;

			tempElement.textName = textName;
			tempElement.bindButton = func;
			tempElement.buttonText = buttonText;
			Buttons.push_back(tempElement);
		}

		void HUDButtons::CreateButton(int posX, int posY, int width, int height, int layerDepth, string name, std::function<void()> func, string textName, string hoverTex, string PressTex, string buttonText)
		{
			ButtonElement tempElement;
			tempElement.PositionX = posX;
			tempElement.PositionY = posY;

			tempElement.Width = width;
			tempElement.Height = height;

			tempElement.layerDepth = layerDepth;
			tempElement.rectName = name;
			tempElement.textName = textName;

			tempElement.hoverTex = hoverTex;
			tempElement.PressTex = PressTex;
			tempElement.bindButton = func;
			tempElement.buttonText = buttonText;
			
			
			Buttons.push_back(tempElement);
		}
		void HUDButtons::CreateButton(int posX, int posY, int width, int height, int layerDepth, string name, std::function<void()> func, unsigned short skillDesc[], string textName, string hoverTex, string PressTex, string buttonText)
		{
			ButtonElement tempElement;
			tempElement.PositionX = posX;
			tempElement.PositionY = posY;

			tempElement.Width = width;
			tempElement.Height = height;

			tempElement.layerDepth = layerDepth;
			tempElement.rectName = name;
			tempElement.textName = textName;

			tempElement.hoverTex = hoverTex;
			tempElement.PressTex = PressTex;
			tempElement.bindButton = func;
			tempElement.buttonText = buttonText;
			tempElement.skillButton = true;
			for (size_t i = 0; i < 7; i++)
			{
				if (skillDesc[i] != 0u)
				{
					tempElement.skillDesc[i] = skillDesc[i];

				}
			}

			Buttons.push_back(tempElement);
		}

		void HUDButtons::DrawButtons()
		{
			for (auto& ButtonElement : Buttons)
			{
				auto entity = CoreInit::managers.entityManager->Create();

				ButtonGuiManager.textureInfo.width = ButtonElement.Width;
				ButtonGuiManager.textureInfo.height = ButtonElement.Height;
				ButtonGuiManager.textureInfo.posX = ButtonElement.PositionX;
				ButtonGuiManager.textureInfo.posY = ButtonElement.PositionY;

				ButtonGuiManager.textureInfo.layerDepth = 1.0f - ButtonElement.layerDepth / 1000.0f;

				ButtonGuiManager.texture = ButtonElement.textName;

				CoreInit::managers.guiManager->Create(entity, ButtonGuiManager);

				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, true);

				ButtonEntityVec.push_back(entity);
				ButtonElement.EntityID = entity.id;

				ButtonElement.EntityIndex = entityIndex;
				entityIndex++;
				if (ButtonElement.buttonText != "")
				{

					DrawButtonText(ButtonElement);
				}
			}
		}

		void HUDButtons::ButtonHover(int mouseX, int mouseY, bool pressed, bool released)
		{
			bool inside = false;
			ButtonElement skillDescBtn;
		
				for (auto& button : Buttons)
				{
					if (button.rectName == "skillDescription")
					{
						skillDescBtn = button;
					}
				}
			
			for (auto& button : Buttons)
			{
				if (mouseX < button.PositionX + button.Width && mouseX > button.PositionX)
				{
					if (mouseY < button.PositionY + button.Height && mouseY > button.PositionY)
					{
						inside = true;
					}
				}

				if (inside)
				{
					
					if (pressed)
					{
						// pressed
						auto& entity = ButtonEntityVec.at(button.EntityIndex);
						if (button.PressTex != "")
						{
							CoreInit::managers.guiManager->SetTexture(entity, button.PressTex);
						}
					}
					else if (released)
					{
						// func
						if (button.bindButton != NULL)
						{
							button.bindButton();
						}
					}
					else
					{
						// hovered;
						auto& entity = ButtonEntityVec.at(button.EntityIndex);
						if (button.hoverTex!="")
						{
							CoreInit::managers.guiManager->SetTexture(entity, button.hoverTex);
						}
						if (button.skillButton)
						{
							if (wasHovering == false)
							{


								auto entText = CoreInit::managers.entityManager->Create();

								std::wstring text;
								std::string habla = "ayy lmao";
								text.assign(habla.begin(), habla.end());

								Graphics::TextGUI guiText;
								guiText.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
								guiText.effect = Graphics::Effect::NoEffect;
								guiText.text = text;
								guiText.hashString = std::hash<std::wstring>()(guiText.text);
								guiText.layerDepth = 0;
								guiText.anchor = DirectX::XMFLOAT2(0, 0);
								guiText.screenAnchor = DirectX::XMFLOAT2(0, 0);
								guiText.posX = skillDescBtn.PositionX;
								guiText.posY = skillDescBtn.PositionY;
								guiText.width = button.Width - 5;
								guiText.height = button.Height - 5;
								guiText.rotation = 0;
								guiText.scale = DirectX::XMFLOAT2(0.9, 0.9);

								CoreInit::managers.textManager->Create(entText, { Utilz::GUID(), guiText });
								CoreInit::managers.textManager->ToggleRenderableText(entText, true);

								skillDescEntity = entText;
								
								entityIndex++;
							}
						}
						wasHovering = true;
					}
				}
				else
				{
					// Idle
					auto& entity = ButtonEntityVec.at(button.EntityIndex);
					CoreInit::managers.guiManager->SetTexture(entity, button.textName);
					if (wasHovering)
					{
						CoreInit::managers.guiManager->ToggleRenderableTexture(skillDescEntity, false);
						CoreInit::managers.entityManager->Destroy(skillDescEntity);
						entityIndex--;
						wasHovering = false;
					}
				}
				inside = false;
			}
		}

		void HUDButtons::DeleteButtons()
		{
			for (auto& entity: ButtonEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, false);
				CoreInit::managers.entityManager->Destroy(entity);
			}
			

		}

		void HUDButtons::HideButtons()
		{
			for (auto& entity : ButtonEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, false);
				
			}
		}

		void HUDButtons::ShowButtons()
		{
			for (auto& entity : ButtonEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, true);
				
			}
		}

		void HUDButtons::DrawButtonText(ButtonElement button)
		{
			auto entText = CoreInit::managers.entityManager->Create();

			std::wstring text;
			text.assign(button.buttonText.begin(), button.buttonText.end());
			
			Graphics::TextGUI guiText;
			guiText.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
			guiText.effect = Graphics::Effect::NoEffect;
			guiText.text = text;
			guiText.hashString = std::hash<std::wstring>()(guiText.text);
			guiText.layerDepth = 0;
			guiText.anchor = DirectX::XMFLOAT2(0, 0);
			guiText.screenAnchor = DirectX::XMFLOAT2(0, 0);
			guiText.posX = button.PositionX;
			guiText.posY = button.PositionY;
			guiText.width = button.Width - 5;
			guiText.height = button.Height -5;
			guiText.rotation = 0;
			guiText.scale = DirectX::XMFLOAT2(0.9, 0.9);

			CoreInit::managers.textManager->Create(entText, { Utilz::GUID(), guiText });
			CoreInit::managers.textManager->ToggleRenderableText(entText, true);

			ButtonEntityVec.push_back(entText);
			button.textEntityID = entText.id;

			button.textEntityIndex = entityIndex;
			entityIndex++;
		}

	}
}

