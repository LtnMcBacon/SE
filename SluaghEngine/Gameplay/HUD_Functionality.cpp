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
			//CalculateScreenPositions();
		}

		HUDButtons::~HUDButtons()
		{
		}

		void HUDButtons::CreateButton(int posX, int posY, int width, int height, int layerDepth, string name, std::function<void()> func, string textName, string hoverTex, string PressTex, string buttonText)
		{
			StartProfile;
			ButtonElement preRender;
			preRender.PositionX = posX;
			preRender.PositionY = posY;
			
			preRender.Width = width;
			preRender.Height = height;
			
			preRender.layerDepth = layerDepth;
			preRender.rectName = name;
			preRender.textName = textName;
		
			preRender.hoverTex = hoverTex;
			preRender.PressTex = PressTex;
			preRender.bindButton = func;
			preRender.buttonText = buttonText;
			preRender.EntityIndex = -1;
			preRenderButtons.push_back(preRender);

			CalculateScreenPositions(width, height,posX,posY);
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
			tempElement.EntityIndex = -1;
			Buttons.push_back(tempElement);
			ProfileReturnVoid;
		}

		void HUDButtons::CreateButton(int posX, int posY, int width, int height, int layerDepth, string name, std::function<void()> func, bool skill, string perkName, string textName, string hoverTex, string PressTex, string buttonText,PerkData perk)
		{
			StartProfile;
			ButtonElement preRender;
			preRender.PositionX = posX;
			preRender.PositionY = posY;

			preRender.Width = width;
			preRender.Height = height;

			preRender.layerDepth = layerDepth;
			preRender.rectName = name;
			preRender.textName = textName;

			preRender.hoverTex = hoverTex;
			preRender.PressTex = PressTex;
			preRender.bindButton = func;
			preRender.buttonText = buttonText;
			preRender.EntityIndex = -1;
			preRenderButtons.push_back(preRender);
			CalculateScreenPositions(width,height,posX,posY);
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
			tempElement.EntityIndex = -1;
			tempElement.perkName = perkName;
			if (skill)
			{
				tempElement.skillButton = true;
			}
			if (perk.name != "")
			{
				tempElement.perk = perk;
			}
			Buttons.push_back(tempElement);
			ProfileReturnVoid;
		}

		void HUDButtons::CreateButton(ButtonElement newButtons)
		{
			StartProfile;
			ButtonElement preRender;
			preRender.PositionX = newButtons.PositionX;
			preRender.PositionY = newButtons.PositionY;

			preRender.Width = newButtons.Width;
			preRender.Height = newButtons.Height;

			preRender.layerDepth = newButtons.layerDepth;
			preRender.rectName = newButtons.rectName;
			preRender.textName = newButtons.textName;

			preRender.hoverTex = newButtons.hoverTex;
			preRender.PressTex = newButtons.PressTex;
			preRender.bindButton = newButtons.bindButton;
			preRender.buttonText = newButtons.buttonText;
			preRender.EntityIndex = -1;
			preRenderButtons.push_back(preRender);

			CalculateScreenPositions(newButtons.Width, newButtons.Height, newButtons.PositionX, newButtons.PositionY);
			ButtonElement tempElement;
			tempElement.PositionX = newButtons.PositionX;
			tempElement.PositionY = newButtons.PositionY;

			tempElement.Width = newButtons.Width;
			tempElement.Height = newButtons.Height;

			tempElement.layerDepth = newButtons.layerDepth;
			tempElement.rectName = newButtons.rectName;
			tempElement.textName = newButtons.textName;

			tempElement.hoverTex = newButtons.hoverTex;
			tempElement.PressTex = newButtons.PressTex;
			tempElement.bindButton = newButtons.bindButton;
			tempElement.buttonText = newButtons.buttonText;
			tempElement.EntityIndex = -1;
			Buttons.push_back(tempElement);
			ProfileReturnVoid;

		}

		void HUDButtons::CreateButton(int posX, int posY, int width, int height, int layerDepth, string name, std::function<void()> func, unsigned short skillDesc[],string skillName, string textName, string hoverTex, string PressTex, string buttonText)
		{
			StartProfile;
			CalculateScreenPositions(width, height,posX,posY);
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
			tempElement.skillName = skillName;
			
			tempElement.EntityIndex = -1;
			for (size_t i = 0; i < 8; i++)
			{
			
				tempElement.skillDesc[i] = skillDesc[i];

			}

			Buttons.push_back(tempElement);
			ProfileReturnVoid;
		}

		std::wstring HUDButtons::printSkillDesc(ButtonElement button)
		{
			StartProfile;
			
			std::string holder;
			std::wstring description;

			if (button.skillName != "")
			{
				holder += button.skillName + "\n";
				std::replace(holder.begin(), holder.end(), '_', ' ');
				description.resize(holder.length(), L'\0');
				std::copy(holder.begin(), holder.end(), description.begin());
				
				description += L"Damage Source: ";
				switch (button.skillDesc[0])
				{
				case 0:
					description += L"Selfcast\n";
					break;
				case 1:
					description += L"Melee\n";
					break;
				case 2:
					description += L"Ranged\n";
					break;
				case 3:
					description += L"Area\n";
					break;
				default:
					description += L"None\n";
					break;
				}

				description += L"Damage Types: ";
				switch (button.skillDesc[1])
				{
				case 0:
					description += L"Physical\n";
					break;
				case 1:
					description += L"Fire\n";
					break;
				case 2:
					description += L"Water\n";
					break;
				case 3:
					description += L"Nature\n";
					break;
				case 4:
					description += L"Ranged\n";
					break;
				case 5:
					description += L"Magic\n";
					break;
				default:
					description += L"None\n";
					break;
				}
				description += L"Boon: ";

				switch (button.skillDesc[2])
				{
				case 0:
					description += L"None\n";
					break;
				case 1:
					description += L"Damage\n";
					break;
				case 2:
					description += L"Knockback\n";
					break;
				case 3:
					description += L"Stun\n";
					break;
				case 4:
					description += L"Root\n";
					break;
				case 5:
					description += L"Protection\n";
					break;
				case 6:
					description += L"Physical Resistance\n";
					break;
				case 7:
					description += L"Magical Resistance\n";
					break;
				case 8:
					description += L"Fire Resistance\n";
					break;
				case 9:
					description += L"Water Resistance";
					break;
				case 10:
					description += L"Nature Resistance\n";
					break;
				case 11:
					description += L"Cast Speed\n";
					break;
				case 12:
					description += L"Swiftness\n";
					break;
				case 13:
					description += L"Slow\n";
					break;
				case 14:
					description += L"Invoulnerability\n";
					break;
				default:
					description += L"None\n";
					break;
				}

				description += L"Banes: ";

				switch (button.skillDesc[3])
				{
				case 0:
					description += L"None\n";
					break;
				case 1:
					description += L"Damage\n";
					break;
				case 2:
					description += L"Stun\n";
					break;
				case 3:
					description += L"Root\n";
					break;
				case 4:
					description += L"Bloodletting\n";
					break;
				case 5:
					description += L"Uncover\n";
					break;
				case 6:
					description += L"Physical Weakness\n";
				case 7:
					description += L"Magical Weakness\n";
					break;
				case 8:
					description += L"Fire Weakness\n";
					break;
				case 9:
					description += L"Water Weakness\n";
					break;
				case 10:
					description += L"Nature Weakness\n";
					break;
				case 11:
					description += L"Slow\n";
					break;
				default:
					description += L"None\n";
					break;
				}
			}
			else
			{
				holder += button.perkName + "\n";
				std::replace(holder.begin(), holder.end(), '_', ' ');
				description.resize(holder.length(), L'\0');
				std::copy(holder.begin(), holder.end(), description.begin());
				

				switch (button.perk.condition)
				{
				case 0:
					description += L"Alltid aktiv\n";
					break;
				case 1:
					description += L"när slagen av fysisk skada\n";
					break;
				case 2:
					description += L"när slagen av vatten skada\n";
					break;
				case 3:
					description += L"när slagen av natur skada\n";
					break;
				case 4:
					description += L"när slagen av magisk skada\n";
					break;
				case 5:
					description += L"när slagen av eld skada\n";
					break;
				case 6:
					description += L"när slåendes med fysisk skada\n";
					break;
				case 7:
					description += L"när slåendes med vatten skada\n";
					break;
				case 8:
					description += L"när slåendes med natur skada\n";
					break;
				case 9:
					description += L"när slåendes med magisk skada\n";
					break;
				case 10:
					description += L"när slåendes med eld skada\n";
					break;
				default:
					break;
				}
				for (size_t i = 0; i < button.perk.typeSize; i++)
				{
					switch (button.perk.types[i])
					{
					case 0:
						description += L"Livs stjälande %: ";
						break;
					case 1:
						description += L"Fysisk resistans %: ";
						break;
					case 2:
						description += L"Vatten resistans %: ";
						break;
					case 3:
						description += L"Natur resistans %: ";
						break;
					case 4:
						description += L"Magisk resistans %: ";
						break;
					case 5:
						description += L"Eld resistans %: ";
						break;
					case 6:
						description += L"Distans resistans %: ";
						break;
					case 7:
						description += L"Styrka adderat: ";
						break;
					case 8:
						description += L"Smidighet adderat: ";
						break;
					case 9:
						description += L"Intelligens adderat: ";
						break;
					case 10:
						description += L"Hälsa adderat: ";
						break;
					case 11:
						description += L"Styrka % ökning: ";
						break;
					case 12:
						description += L"Smidighet % ökning: ";
						break;
					case 13:
						description += L"Intelligens % ökning: ";
						break;
					case 14:
						description += L"Hälsa % ökning: ";
						break;
					case 15:
						description += L"Skada % ökning: ";
						break;
					case 16:
						description += L"Närstrids längd % ökning: ";
						break;
					case 17:
						description += L"Närstrids skada % ökning: ";
						break;
					case 18:
						description += L"Distans skada % ökning: ";
						break;
					case 19:
						description += L"Attack hastighet % ökning: ";
						break;
					case 20:
						description += L"Närstrids attack hastighet % ökning: ";
						break;
					case 21:
						description += L"Skada % ökning per slag: ";
						break;
					case 22:
						description += L"Attack hastighet % ökning per slag: ";
						break;
					case 23:
						description += L"Närstrids skada % ökning per slag: ";
						break;
					case 24:
						description += L"Distans skada % ökning per slag: ";
						break;
					case 25:
						description += L"Närstrids attack hastighet % ökning per slag: ";
						break;
					case 26:
						description += L"Distans attack hastighet % ökning per slag: ";
						break;
					case 27:
						description += L"inte aktuell!\n";
						break;
					case 28:
						description += L"Förmåga nedkylning %: ";
						break;
					case 29:
						description += L"Förmåga nedkylning sekunder: ";
						break;
					case 30:
						description += L"Förmåga skada % ökning: ";
						break;
					case 31:
						description += L"Momentum % ökning: ";
						break;
					case 32:
						description += L"Max hälsa % helande: ";
						break;
					case 33:
						description += L"Hälsa helande:\n";
						break;
					case 34:
						description += L"Helande immun\n";
						break;
					case 35:
						description += L"Fysiskt immun\n";
						break;
					case 36:
						description += L"Vatten immun\n";
						break;
					case 37:
						description += L"Eld immun\n";
						break;
					case 38:
						description += L"Natur immun\n";
						break;
					case 39:
						description += L"Nedslöandes immun\n";
						break;
					case 40:
						description += L"Bedövnings immun\n";
						break;
					case 41:
						description += L"Magi immun\n";
						break;
					case 42:
						description += L"Puttnings immun\n";
						break;
					case 43:
						description += L"Närstrid Låst\n";
						break;
					case 44:
						description += L"Distans Låst\n";
						break;
					case 45:
						description += L"Magi Låst\n";
						break;
					case 46:
						description += L"Vatten Låst\n";
						break;
					case 47:
						description += L"Eld Låst\n";
						break;
					case 48:
						description += L"Natur Låst\n";
						break;
					default:
						break;
					}
					description += std::to_wstring(button.perk.values[i]) + L"\n";
				}

				for (size_t i = 0; i < button.perk.checkSize; i++)
				{
					switch (button.perk.checks[i])
					{
					case 0:
						description += L"Livs stjälande: ";
						break;
					case 1:
						description += L"Fysisk resistans: ";
						break;
					case 2:
						description += L"Vatten resistans: ";
						break;
					case 3:
						description += L"Natur resistans: ";
						break;
					case 4:
						description += L"Magisk resistans: ";
						break;
					case 5:
						description += L"Eld resistans: ";
						break;
					case 6:
						description += L"Distans resistans: ";
						break;
					case 7:
						description += L"Styrka adderat: ";
						break;
					case 8:
						description += L"Smidighet adderat: ";
						break;
					case 9:
						description += L"Intelligens adderat: ";
						break;
					case 10:
						description += L"Hälsa adderat: ";
						break;
					case 11:
						description += L"Styrka % ökning: ";
						break;
					case 12:
						description += L"Smidighet % ökning: ";
						break;
					case 13:
						description += L"Intelligens % ökning: ";
						break;
					case 14:
						description += L"Hälsa % ökning: ";
						break;
					case 15:
						description += L"Skada % ökning: ";
						break;
					case 16:
						description += L"Närstrids längd % ökning: ";
						break;
					case 17:
						description += L"Närstrids skada % ökning: ";
						break;
					case 18:
						description += L"Distans skada % ökning: ";
						break;
					case 19:
						description += L"Attack hastighet % ökning: ";
						break;
					case 20:
						description += L"Närstrids attack hastighet % ökning: ";
						break;
					case 21:
						description += L"Skada % ökning per slag: ";
						break;
					case 22:
						description += L"Attack hastighet % ökning per slag: ";
						break;
					case 23:
						description += L"Närstrids skada % ökning per slag: ";
						break;
					case 24:
						description += L"Distans skada % ökning per slag: ";
						break;
					case 25:
						description += L"Närstrids attack hastighet % ökning per slag: ";
						break;
					case 26:
						description += L"Distans attack hastighet % ökning per slag: ";
						break;
					case 27:
						description += L"inte aktuell!\n";
						break;
					case 28:
						description += L"Förmåga nedkylning %:";
						break;
					case 29:
						description += L"Förmåga nedkylning sekunder: ";
						break;
					case 30:
						description += L"Förmåga skada % ökning: ";
						break;
					case 31:
						description += L"Momentum % ökning: ";
						break;
					case 32:
						description += L"Max hälsa % helande: ";
						break;
					case 33:
						description += L"Hälsa helande: ";
						break;
					case 34:
						description += L"Helande immun\n";
						break;
					case 35:
						description += L"Fysiskt immun\n";
						break;
					case 36:
						description += L"Vatten immun\n";
						break;
					case 37:
						description += L"Eld immun\n";
						break;
					case 38:
						description += L"Natur immun\n";
						break;
					case 39:
						description += L"Nedslöandes immun\n";
						break;
					case 40:
						description += L"Bedövnings immun\n";
						break;
					case 41:
						description += L"Magi immun\n";
						break;
					case 42:
						description += L"Puttnings immun\n";
						break;
					case 43:
						description += L"Närstrid Låst\n";
						break;
					case 44:
						description += L"Distans Låst\n";
						break;
					case 45:
						description += L"Magi Låst\n";
						break;
					case 46:
						description += L"Vatten Låst\n";
						break;
					case 47:
						description += L"Eld Låst";
						break;
					case 48:
						description += L"Natur Låst";
						break;
					default:
						break;
					}

				}

			}

			ProfileReturnConst(description);
		}

		void HUDButtons::DrawButtons()
		{
			StartProfile;
			for (auto& ButtonElement : Buttons)
			{
				if (ButtonElement.EntityIndex != -1)
				{
					continue;
				}

				bool perhaps = false;
				bool fullscreen = CoreInit::subSystems.optionsHandler->GetOptionBool("Window", "fullScreen", perhaps);
				if (fullscreen)
				{
					reverseScreenPositions(ButtonElement.Width, ButtonElement.Height,ButtonElement.PositionX,ButtonElement.PositionY);
				}
				

				auto entity = CoreInit::managers.entityManager->Create();
				if (fullscreen)
				{

					ButtonGuiManager.textureInfo.width = Width;
					ButtonGuiManager.textureInfo.height = Height;
					ButtonGuiManager.textureInfo.posX = positionX;
					ButtonGuiManager.textureInfo.posY = positionY;
				}
				else
				{
					ButtonGuiManager.textureInfo.width = ButtonElement.Width;
					ButtonGuiManager.textureInfo.height = ButtonElement.Height;
					ButtonGuiManager.textureInfo.posX = ButtonElement.PositionX;
					ButtonGuiManager.textureInfo.posY = ButtonElement.PositionY;
				}
				ButtonGuiManager.textureInfo.absolute = true;

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

				if (ButtonElement.skillButton)
				{
					SkillNPerkEntityVec.push_back(entity);
					ButtonsToDelete.push_back(ButtonElement.rectName);
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
						break;
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
						// function body
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
								std::wstring text;
								text = printSkillDesc(button);
								auto entText = CoreInit::managers.entityManager->Create();
								
								

								bool isFullscreen = false;
								isFullscreen = CoreInit::subSystems.optionsHandler->GetOptionBool("Window", "fullScreen", isFullscreen);

								if (isFullscreen)
								{
									reverseScreenPositions(skillDescBtn.Width, skillDescBtn.Height, skillDescBtn.PositionX, skillDescBtn.PositionY,true);
								}
								Graphics::TextGUI guiText;
								guiText.colour = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 1.0);
								guiText.effect = Graphics::Effect::NoEffect;
								guiText.text = text;
								
								guiText.hashString = std::hash<std::wstring>()(guiText.text);
								guiText.layerDepth = 0;
								guiText.anchor = DirectX::XMFLOAT2(0, 0);
								guiText.screenAnchor = DirectX::XMFLOAT2(0, 0);
								guiText.posX = skillDescBtn.PositionX+55;
								guiText.posY = skillDescBtn.PositionY+30;
								guiText.width = skillDescBtn.Width-50;
								guiText.height = skillDescBtn.Height-10;
								guiText.rotation = 0;
								
								guiText.scale = DirectX::XMFLOAT2(0.9, 0.9);

								CoreInit::managers.textManager->Create(entText, { Utilz::GUID("EnchantedLand.spritefont"), guiText });
								CoreInit::managers.textManager->ToggleRenderableText(entText, true);

								skillDescEntity = entText;
								
								
							}
						}
						if (button.rectName != "skillBackgroundBtn" && button.rectName != "skillBackgroundBtn2" && button.rectName != "skillBackgroundBtn3")
						{
							wasHovering = true;
							break;
						}
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
			entityIndex = 0;
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

		void HUDButtons::deleteSkillPerkBtns()
		{
			StartProfile;
			for (int i = 0; i < Buttons.size(); i++)
			{

				for (auto& name : ButtonsToDelete)
				{
					if (name == Buttons[i].rectName)
					{
						Buttons.erase(Buttons.begin()+i);
						i = 0;
						
					}
				}
			}
			
			for (auto& entity : SkillNPerkEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, false);
				CoreInit::managers.entityManager->Destroy(entity);
			}
			SkillNPerkEntityVec.clear();

			if (skillDescEntity.id != 0)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(skillDescEntity, false);
				CoreInit::managers.entityManager->Destroy(skillDescEntity);
			}

			ProfileReturnVoid;
		}

		void HUDButtons::CalculateScreenPositions(int& width, int& height, int& posX, int& posY)
		{
			size_t screenHeight = CoreInit::subSystems.window->Height();
			size_t screenWidth = CoreInit::subSystems.window->Width();

			float minWidth = 1280;
			float minHeight = 720;
			float newWidthPercent = screenWidth / minWidth;
			float newHeightPercent = screenHeight / minHeight;

			posX = posX*newWidthPercent;
			posY = posY*newHeightPercent;
			width =  width*newWidthPercent;
			height = height*newHeightPercent;

		}

		void HUDButtons::reverseScreenPositions(int width, int height,int posX, int posY)
		{
			size_t screenHeight = CoreInit::subSystems.window->Height();
			size_t screenWidth = CoreInit::subSystems.window->Width();

			float minWidth = 1280;
			float minHeight = 720;

			float newWidthPercent = screenWidth / minWidth;
			float newHeightPercent = screenHeight / minHeight;

			positionX = posX/newWidthPercent;
			positionY = posY / newWidthPercent;
			Width = width/newWidthPercent;
			Height = height/newHeightPercent;
		}
		void HUDButtons::reverseScreenPositions(int& width, int& height, int& posX, int& posY,bool reference)
		{
			size_t screenHeight = CoreInit::subSystems.window->Height();
			size_t screenWidth = CoreInit::subSystems.window->Width();

			float minWidth = 1280;
			float minHeight = 720;

			float newWidthPercent = screenWidth / minWidth;
			float newHeightPercent = screenHeight / minHeight;

			posX = posX / newWidthPercent;
			posY = posY / newWidthPercent;
			width = width / newWidthPercent;
			height = height / newHeightPercent;
		}

		void HUDButtons::DrawButtonText(ButtonElement &button)
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
			guiText.posX = button.PositionX -5;
			guiText.posY = button.PositionY -5;
			guiText.width = button.Width -5;
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

		void HUDButtons::DeleteSpecificButtons(string name)
		{
			for (auto& Button: Buttons)
			{
				if (Button.rectName == name)
				{
					CoreInit::managers.guiManager->ToggleRenderableTexture(ButtonEntityVec.at(Button.EntityIndex),false);
					CoreInit::managers.entityManager->Destroy(ButtonEntityVec.at(Button.EntityIndex));
				}
			}
		}

		void HUDButtons::reloadButtons()
		{
			StartProfile;
			DeleteButtons();
			std::vector<ButtonElement> newButtons = preRenderButtons;
			preRenderButtons.clear();
			Buttons.clear();
			for (auto& Button: newButtons)
			{
				CreateButton(Button);
			}
			DrawButtons();
			ProfileReturnVoid;
		}

	}
}

