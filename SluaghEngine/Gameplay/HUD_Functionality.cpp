#include "HUD_Functionality.h"
#include <HUD_Parsing.h>
#include "CoreInit.h"
#include <Profiler.h>
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
			StartProfile;
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
			ProfileReturnVoid;
		}

		void HUDButtons::CreateButton(int posX, int posY, int width, int height, int layerDepth, string name, std::function<void()> func, string textName, string hoverTex, string PressTex, string buttonText)
		{
			StartProfile;
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
			ProfileReturnVoid;
		}
		void HUDButtons::CreateButton(int posX, int posY, int width, int height, int layerDepth, string name, std::function<void()> func, unsigned short skillDesc[], string textName, string hoverTex, string PressTex, string buttonText)
		{
			StartProfile;
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
			ProfileReturnVoid;
		}

		std::string HUDButtons::printSkillDesc(ButtonElement button)
		{
			StartProfile;
			std::string description = "";
			description += "Damage Source: ";
			switch (button.skillDesc[0])
			{
			case 0:
				description += "Selfcast\n";
				break;
			case 1:
				description += "Melee\n";
				break;
			case 2:
				description += "Ranged\n";
				break;
			case 3:
				description += "Area\n";
				break;
			}

			description += "Damage Types: ";
			switch (button.skillDesc[1])
			{
			case 0:
				description += "Physical\n";
				break;
			case 1:
				description += "Magical\n";
				break;
			case 2:
				description += "Fire\n";
				break;
			case 3:
				description += "Water\n";
				break;
			case 4:
				description += "Nature\n";
				break;
			}
			description += "Boon: ";

			switch (button.skillDesc[2])
			{
			case 0:
				description += "None\n";
				break;
			case 1:
				description += "Damage\n";
				break;
			case 2:
				description += "Knockback\n";
				break;
			case 3:
				description += "Stun\n";
				break;
			case 4:
				description += "Root\n";
				break;
			case 5:
				description += "Protection\n";
				break;
			case 6:
				description += "Physical Resistance\n";
				break;
			case 7:
				description += "Magical Resistance\n";
				break;
			case 8:
				description += "Fire Resistance\n";
				break;
			case 9:
				description += "Water Resistance";
				break;
			case 10:
				description += "Nature Resistance\n";
				break;
			case 11:
				description += "Cast Speed\n";
				break;
			case 12:
				description += "Swiftness\n";
				break;
			case 13:
				description += "Slow\n";
				break;
			case 14:
				description += "Invoulnerability\n";
				break;
			}

			description += "Banes: ";

			switch (button.skillDesc[3])
			{
			case 0:
				description += "None\n";
				break;
			case 1:
				description += "Damage\n";
				break;
			case 2:
				description += "Stun\n";
				break;
			case 3:
				description += "Root\n";
				break;
			case 4:
				description += "Bloodletting\n";
				break;
			case 5:
				description += "Uncover\n";
				break;
			case 6: 
				description += "Physical Weakness\n";
			case 7:
				description += "Magical Weakness\n";
				break;
			case 8:
				description += "Fire Weakness\n";
				break;
			case 9:
				description += "Water Weakness\n";
				break;
			case 10:
				description += "Nature Weakness\n";
				break;
			case 11:
				description += "Slow\n";
				break;
			}

			
			ProfileReturnConst(description);
		}

		void HUDButtons::DrawButtons()
		{
			StartProfile;
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
			ProfileReturnVoid;
		}

		void HUDButtons::ButtonHover(int mouseX, int mouseY, bool pressed, bool released)
		{
			StartProfile;
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
						if (button.skillButton)
						{
							wasHovering = true;
						}
					}
					else if (released)
					{
						// func
						if (button.skillButton)
						{
							wasHovering = true;
						}
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
							std::string holder = printSkillDesc(button);
								auto entText = CoreInit::managers.entityManager->Create();

								std::wstring text;
								
								text.assign(holder.begin(), holder.end());

								Graphics::TextGUI guiText;
								guiText.colour = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 1.0);
								guiText.effect = Graphics::Effect::NoEffect;
								guiText.text = text;
								
								guiText.hashString = std::hash<std::wstring>()(guiText.text);
								guiText.layerDepth = 0;
								guiText.anchor = DirectX::XMFLOAT2(0, 0);
								guiText.screenAnchor = DirectX::XMFLOAT2(0, 0);
								guiText.posX = skillDescBtn.PositionX;
								guiText.posY = skillDescBtn.PositionY;
								guiText.width = skillDescBtn.Width;
								guiText.height = skillDescBtn.Height;
								guiText.rotation = 0;
								
								guiText.scale = DirectX::XMFLOAT2(0.9, 0.9);

								CoreInit::managers.textManager->Create(entText, { Utilz::GUID("CloisterBlack.spritefont"), guiText });
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
					if (button.skillButton)
					{

						if (wasHovering)
						{
							if (skillDescEntity.id != 0)
							{

								CoreInit::managers.guiManager->ToggleRenderableTexture(skillDescEntity, false);
								CoreInit::managers.entityManager->Destroy(skillDescEntity);
								entityIndex--;
							}
							wasHovering = false;
						}
					}
				}
				inside = false;
			}
			ProfileReturnVoid;
		}

		void HUDButtons::DeleteButtons()
		{
			StartProfile;
			for (auto& entity: ButtonEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, false);
				CoreInit::managers.entityManager->Destroy(entity);
			}
			
			ProfileReturnVoid;
		}

		void HUDButtons::HideButtons()
		{
			StartProfile;
			for (auto& entity : ButtonEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, false);
				
			}
			ProfileReturnVoid;
		}

		void HUDButtons::ShowButtons()
		{
			StartProfile;
			for (auto& entity : ButtonEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, true);
				
			}
			ProfileReturnVoid;
		}

		void HUDButtons::DrawButtonText(ButtonElement button)
		{
			StartProfile;
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
			ProfileReturnVoid;
		}

	}
}

