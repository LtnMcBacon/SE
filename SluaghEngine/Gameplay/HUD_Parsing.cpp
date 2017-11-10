#include <HUD_Parsing.h>
#include "CoreInit.h"
using namespace std;


namespace SE
{
	namespace Gameplay
	{
		HUDParser::HUDParser()
		{
			std::function<void()> test = std::bind(&HUDParser::InitiateTextures, this);
			test();
			this->InitiateTextures();
		}

		HUDParser::~HUDParser()
		{
			for (auto& entities: texEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entities, false);
				CoreInit::managers.entityManager->Destroy(entities);
			}
		}

		void HUDParser::ParseFiles(Utilz::GUID fileName)
		{
		

			int nrOfRects = 0;
			int nrOfTexts = 0;

			CoreInit::subSystems.resourceHandler->LoadResource(fileName, [&nrOfRects, &nrOfTexts,this](auto guid, auto data, auto size)
			{
				int head = 0;
				int loader = 0;
				int strLength = 0;
				HUDElement CurrentElm;
				HUDText currentText;
				char* arr = nullptr;

				//	NUMBER OF RECTANGLES
				memcpy(&nrOfRects, data, sizeof(int));
				head += sizeof(int);
				//	NUMBER OF TEXT ELEMENTS
				memcpy(&nrOfTexts, (char*)data + head, sizeof(int));
				head += sizeof(int);

				// PARSE TEXTURES
				for (size_t i = 0; i < nrOfRects; i++)
				{

					// SIZE OF RECT NAME
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					// RECT NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					CurrentElm.rectName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;


					// SIZE OF TEXTURE NAME
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					// TEXTURE NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					CurrentElm.textName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	RECTANGLE WIDTH
					memcpy(&CurrentElm.Width, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE HEIGHT
					memcpy(&CurrentElm.Height, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE POS X
					memcpy(&CurrentElm.PositionX, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE POS Y
					memcpy(&CurrentElm.PositionY, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE BUTTON BOOL
					memcpy(&CurrentElm.Btn, (char*)data + head, sizeof(bool));
					head += sizeof(bool);

					if (CurrentElm.Btn)
					{
						//	HOVERTEX NAME LENGTH
						memcpy(&strLength, (char*)data + head, sizeof(int));
						head += sizeof(int);
						head += 1;

						//	HOVERTEX NAME
						arr = new char[strLength + 1];
						memcpy(arr, (char*)data + head, strLength);
						arr[strLength] = '\0';
						CurrentElm.hoverTex = arr;
						head += strLength;
						strLength = 0;
						delete[] arr;

						//	PRESSTEX NAME LENGTH
						memcpy(&strLength, (char*)data + head, sizeof(int));
						head += sizeof(int);
						head += 1;

						//	PRESSTEX NAME
						arr = new char[strLength + 1];
						memcpy(arr, (char*)data + head, strLength);
						arr[strLength] = '\0';
						CurrentElm.PressTex = arr;
						head += strLength;
						strLength = 0;
						delete[] arr;
					}

					//	OPACITY
					memcpy(&CurrentElm.Opacity, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	LAYERDEPTH
					memcpy(&CurrentElm.layerDepth, (char*)data + head, sizeof(int));
					head += sizeof(int);
					if (CurrentElm.Btn)
					{
						this->ButtonVector.push_back(CurrentElm);
					}
					else
					{

						this->MainMenuElmVec.push_back(CurrentElm);
					}
					
				}

				// PARSE TEXTS
				for (size_t l = 0; l < nrOfTexts; l++)
				{

					//	TEXT NAME LENGTH
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					//	TEXT NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					currentText.TextName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	TEXT CONTENT LENGTH
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					//	TEXT CONTENT
					char* arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					currentText.TextContent = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;
					
					//	TEXT HEIGHT
					memcpy(&currentText.Height, (char*)data + head, sizeof(int));
					head += sizeof(int);
					

					//	TEXT WIDTH
					memcpy(&currentText.Width, (char*)data + head, sizeof(int));
					head += sizeof(int);
					

					//	TEXT POS X
					memcpy(&currentText.PosX, (char*)data + head, sizeof(int));
					head += sizeof(int);
					

					//	TEXT POS Y
					memcpy(&currentText.PosY, (char*)data + head, sizeof(int));
					head += sizeof(int);
					
					this->MainMenuTextVec.push_back(currentText);
				}

				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
			});

		}

		void HUDParser::InitiateTextures()
		{
			
			
			for (auto& HUDElement : MainMenuElmVec)
			{
				auto entity = CoreInit::managers.entityManager->Create();
				
				GuiManager.textureInfo.width = HUDElement.Width;
				GuiManager.textureInfo.height = HUDElement.Height;
				GuiManager.textureInfo.posX = HUDElement.PositionX;
				GuiManager.textureInfo.posY = HUDElement.PositionY;

				GuiManager.textureInfo.layerDepth = 1.0f - HUDElement.layerDepth / 1000.0f;

				GuiManager.texture = HUDElement.textName;

				CoreInit::managers.guiManager->Create(entity, GuiManager);

				CoreInit::managers.guiManager->ToggleRenderableTexture(entity, true);
			
				texEntityVec.push_back(entity);
				HUDElement.EntityID = entity.id;
				
				HUDElement.EntityIndex = entityIndex;
				entityIndex++;

			}
			for (auto& HUDText : MainMenuTextVec)
			{

			}
		}

		void HUDParser::checkPressed(bool pressed, int mousePosX, int mousePosY)
		{
			for (size_t i = 0; i < MainMenuElmVec.size(); i++)
			{
				HUDElement HUDs = MainMenuElmVec.at(i);

				if (HUDs.Btn)
				{
					HoverButton(pressed,mousePosX, mousePosY,HUDs);
				}
			}

			
		}

		void HUDParser::HoverButton(bool pressed, int mousePosX, int mousePosY,HUDElement HUDButton)
		{
			bool inside = false;

			if (mousePosX < HUDButton.PositionX + HUDButton.Width && mousePosX > HUDButton.PositionX)
			{
				if (mousePosY < HUDButton.PositionY + HUDButton.Height && mousePosY > HUDButton.PositionY)
				{
					inside = true;
				}
			}

			if (inside)
			{
				if (pressed)
				{
					texPressed(HUDButton);
				}
				else
				{
					texHovered(HUDButton);
				}
			}
			else
			{
				texIdle(HUDButton);
			}
		}

		void HUDParser::texHovered(HUDElement HUDButton)
		{
			auto& entity = texEntityVec.at(HUDButton.EntityIndex);
			CoreInit::managers.guiManager->SetTexture(entity, HUDButton.hoverTex);
			
			
		}

		void HUDParser::texPressed(HUDElement HUDButton)
		{
			auto& entity = texEntityVec.at(HUDButton.EntityIndex);
			CoreInit::managers.guiManager->SetTexture(entity, HUDButton.PressTex);
			if (HUDButton.bindButton != NULL)
			{
				HUDButton.bindButton();
			}
		}

		void HUDParser::texIdle(HUDElement HUDButton)
		{
			auto& entity = texEntityVec.at(HUDButton.EntityIndex);
			CoreInit::managers.guiManager->SetTexture(entity, HUDButton.textName);
			
		}

		void HUDParser::SetFunctionOnPress(string identifier, function<void()> func)
		{
			for (auto& HUDElements : MainMenuElmVec)
			{

				if (HUDElements.rectName == identifier)
				{
					HUDElements.bindButton = func;
				}


			}
		}

		void HUDParser::ParseSkillButtons(Utilz::GUID fileName)
		{
			
			int nrOfRects = 0;
			int nrOfTexts = 0;

			CoreInit::subSystems.resourceHandler->LoadResource(fileName, [&nrOfRects, &nrOfTexts, this](auto guid, auto data, auto size)
			{
				int head = 0;
				int loader = 0;
				int strLength = 0;
				HUDElement CurrentElm;
				HUDText currentText;
				char* arr = nullptr;

				//	NUMBER OF RECTANGLES
				memcpy(&nrOfRects, data, sizeof(int));
				head += sizeof(int);
				//	NUMBER OF TEXT ELEMENTS
				memcpy(&nrOfTexts, (char*)data + head, sizeof(int));
				head += sizeof(int);

				// PARSE TEXTURES
				for (size_t i = 0; i < nrOfRects; i++)
				{

					// SIZE OF RECT NAME
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					// RECT NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					CurrentElm.rectName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;


					// SIZE OF TEXTURE NAME
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					// TEXTURE NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					CurrentElm.textName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	RECTANGLE WIDTH
					memcpy(&CurrentElm.Width, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE HEIGHT
					memcpy(&CurrentElm.Height, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE POS X
					memcpy(&CurrentElm.PositionX, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE POS Y
					memcpy(&CurrentElm.PositionY, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE BUTTON BOOL
					memcpy(&CurrentElm.Btn, (char*)data + head, sizeof(bool));
					head += sizeof(bool);

					if (CurrentElm.Btn)
					{
						//	HOVERTEX NAME LENGTH
						memcpy(&strLength, (char*)data + head, sizeof(int));
						head += sizeof(int);
						head += 1;

						//	HOVERTEX NAME
						arr = new char[strLength + 1];
						memcpy(arr, (char*)data + head, strLength);
						arr[strLength] = '\0';
						CurrentElm.hoverTex = arr;
						head += strLength;
						strLength = 0;
						delete[] arr;

						//	PRESSTEX NAME LENGTH
						memcpy(&strLength, (char*)data + head, sizeof(int));
						head += sizeof(int);
						head += 1;

						//	PRESSTEX NAME
						arr = new char[strLength + 1];
						memcpy(arr, (char*)data + head, strLength);
						arr[strLength] = '\0';
						CurrentElm.PressTex = arr;
						head += strLength;
						strLength = 0;
						delete[] arr;
					}

					//	OPACITY
					memcpy(&CurrentElm.Opacity, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	LAYERDEPTH
					memcpy(&CurrentElm.layerDepth, (char*)data + head, sizeof(int));
					head += sizeof(int);

					this->skillButtonVec.push_back(CurrentElm);

				}

				// PARSE TEXTS
				for (size_t l = 0; l < nrOfTexts; l++)
				{

					//	TEXT NAME LENGTH
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					//	TEXT NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					currentText.TextName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	TEXT CONTENT LENGTH
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					//	TEXT CONTENT
					char* arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					currentText.TextContent = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	TEXT HEIGHT
					memcpy(&currentText.Height, (char*)data + head, sizeof(int));
					head += sizeof(int);


					//	TEXT WIDTH
					memcpy(&currentText.Width, (char*)data + head, sizeof(int));
					head += sizeof(int);


					//	TEXT POS X
					memcpy(&currentText.PosX, (char*)data + head, sizeof(int));
					head += sizeof(int);


					//	TEXT POS Y
					memcpy(&currentText.PosY, (char*)data + head, sizeof(int));
					head += sizeof(int);

					this->skillTextVec.push_back(currentText);
				}

				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
			});
		}

		void HUDParser::ParsePerks(Utilz::GUID fileName)
		{
			int nrOfRects = 0;
			int nrOfTexts = 0;

			CoreInit::subSystems.resourceHandler->LoadResource(fileName, [&nrOfRects, &nrOfTexts, this](auto guid, auto data, auto size)
			{
				int head = 0;
				int loader = 0;
				int strLength = 0;
				HUDElement CurrentElm;
				HUDText currentText;
				char* arr = nullptr;

				//	NUMBER OF RECTANGLES
				memcpy(&nrOfRects, data, sizeof(int));
				head += sizeof(int);
				//	NUMBER OF TEXT ELEMENTS
				memcpy(&nrOfTexts, (char*)data + head, sizeof(int));
				head += sizeof(int);

				// PARSE TEXTURES
				for (size_t i = 0; i < nrOfRects; i++)
				{

					// SIZE OF RECT NAME
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					// RECT NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					CurrentElm.rectName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;


					// SIZE OF TEXTURE NAME
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					// TEXTURE NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					CurrentElm.textName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	RECTANGLE WIDTH
					memcpy(&CurrentElm.Width, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE HEIGHT
					memcpy(&CurrentElm.Height, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE POS X
					memcpy(&CurrentElm.PositionX, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE POS Y
					memcpy(&CurrentElm.PositionY, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE BUTTON BOOL
					memcpy(&CurrentElm.Btn, (char*)data + head, sizeof(bool));
					head += sizeof(bool);

					if (CurrentElm.Btn)
					{
						//	HOVERTEX NAME LENGTH
						memcpy(&strLength, (char*)data + head, sizeof(int));
						head += sizeof(int);
						head += 1;

						//	HOVERTEX NAME
						arr = new char[strLength + 1];
						memcpy(arr, (char*)data + head, strLength);
						arr[strLength] = '\0';
						CurrentElm.hoverTex = arr;
						head += strLength;
						strLength = 0;
						delete[] arr;

						//	PRESSTEX NAME LENGTH
						memcpy(&strLength, (char*)data + head, sizeof(int));
						head += sizeof(int);
						head += 1;

						//	PRESSTEX NAME
						arr = new char[strLength + 1];
						memcpy(arr, (char*)data + head, strLength);
						arr[strLength] = '\0';
						CurrentElm.PressTex = arr;
						head += strLength;
						strLength = 0;
						delete[] arr;
					}

					//	OPACITY
					memcpy(&CurrentElm.Opacity, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	LAYERDEPTH
					memcpy(&CurrentElm.layerDepth, (char*)data + head, sizeof(int));
					head += sizeof(int);

					this->perkButtonVec.push_back(CurrentElm);

				}

				// PARSE TEXTS
				for (size_t l = 0; l < nrOfTexts; l++)
				{

					//	TEXT NAME LENGTH
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					//	TEXT NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					currentText.TextName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	TEXT CONTENT LENGTH
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					//	TEXT CONTENT
					char* arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					currentText.TextContent = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	TEXT HEIGHT
					memcpy(&currentText.Height, (char*)data + head, sizeof(int));
					head += sizeof(int);


					//	TEXT WIDTH
					memcpy(&currentText.Width, (char*)data + head, sizeof(int));
					head += sizeof(int);


					//	TEXT POS X
					memcpy(&currentText.PosX, (char*)data + head, sizeof(int));
					head += sizeof(int);


					//	TEXT POS Y
					memcpy(&currentText.PosY, (char*)data + head, sizeof(int));
					head += sizeof(int);

					this->perkTextVec.push_back(currentText);
				}

				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
			});
		}

		void HUDParser::ParseOptionMenu(Utilz::GUID fileName)
		{


			int nrOfRects = 0;
			int nrOfTexts = 0;

			CoreInit::subSystems.resourceHandler->LoadResource(fileName, [&nrOfRects, &nrOfTexts, this](auto guid, auto data, auto size)
			{
				int head = 0;
				int loader = 0;
				int strLength = 0;
				HUDElement CurrentElm;
				HUDText currentText;
				char* arr = nullptr;

				//	NUMBER OF RECTANGLES
				memcpy(&nrOfRects, data, sizeof(int));
				head += sizeof(int);
				//	NUMBER OF TEXT ELEMENTS
				memcpy(&nrOfTexts, (char*)data + head, sizeof(int));
				head += sizeof(int);

				// PARSE TEXTURES
				for (size_t i = 0; i < nrOfRects; i++)
				{

					// SIZE OF RECT NAME
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					// RECT NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					CurrentElm.rectName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;


					// SIZE OF TEXTURE NAME
					memcpy(&strLength, (char*)data + head, sizeof(int));
					head += sizeof(int);
					head += 1;

					// TEXTURE NAME
					arr = new char[strLength + 1];
					memcpy(arr, (char*)data + head, strLength);
					arr[strLength] = '\0';
					CurrentElm.textName = arr;
					head += strLength;
					strLength = 0;
					delete[] arr;

					//	RECTANGLE WIDTH
					memcpy(&CurrentElm.Width, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE HEIGHT
					memcpy(&CurrentElm.Height, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE POS X
					memcpy(&CurrentElm.PositionX, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE POS Y
					memcpy(&CurrentElm.PositionY, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	RECTANGLE BUTTON BOOL
					memcpy(&CurrentElm.Btn, (char*)data + head, sizeof(bool));
					head += sizeof(bool);

					if (CurrentElm.Btn)
					{
						//	HOVERTEX NAME LENGTH
						memcpy(&strLength, (char*)data + head, sizeof(int));
						head += sizeof(int);
						head += 1;

						//	HOVERTEX NAME
						arr = new char[strLength + 1];
						memcpy(arr, (char*)data + head, strLength);
						arr[strLength] = '\0';
						CurrentElm.hoverTex = arr;
						head += strLength;
						strLength = 0;
						delete[] arr;

						//	PRESSTEX NAME LENGTH
						memcpy(&strLength, (char*)data + head, sizeof(int));
						head += sizeof(int);
						head += 1;

						//	PRESSTEX NAME
						arr = new char[strLength + 1];
						memcpy(arr, (char*)data + head, strLength);
						arr[strLength] = '\0';
						CurrentElm.PressTex = arr;
						head += strLength;
						strLength = 0;
						delete[] arr;
					}

					//	OPACITY
					memcpy(&CurrentElm.Opacity, (char*)data + head, sizeof(int));
					head += sizeof(int);

					//	LAYERDEPTH
					memcpy(&CurrentElm.layerDepth, (char*)data + head, sizeof(int));
					head += sizeof(int);
					if (CurrentElm.Btn)
					{
						optionButtons.push_back(CurrentElm);
					}
					else
					{
						optionsMenu.push_back(CurrentElm);
					}

				}

				//// PARSE TEXTS
				//for (size_t l = 0; l < nrOfTexts; l++)
				//{
				//
				//	//	TEXT NAME LENGTH
				//	memcpy(&strLength, (char*)data + head, sizeof(int));
				//	head += sizeof(int);
				//	head += 1;
				//
				//	//	TEXT NAME
				//	arr = new char[strLength + 1];
				//	memcpy(arr, (char*)data + head, strLength);
				//	arr[strLength] = '\0';
				//	currentText.TextName = arr;
				//	head += strLength;
				//	strLength = 0;
				//	delete[] arr;
				//
				//	//	TEXT CONTENT LENGTH
				//	memcpy(&strLength, (char*)data + head, sizeof(int));
				//	head += sizeof(int);
				//	head += 1;
				//
				//	//	TEXT CONTENT
				//	char* arr = new char[strLength + 1];
				//	memcpy(arr, (char*)data + head, strLength);
				//	arr[strLength] = '\0';
				//	currentText.TextContent = arr;
				//	head += strLength;
				//	strLength = 0;
				//	delete[] arr;
				//
				//	//	TEXT HEIGHT
				//	memcpy(&currentText.Height, (char*)data + head, sizeof(int));
				//	head += sizeof(int);
				//
				//
				//	//	TEXT WIDTH
				//	memcpy(&currentText.Width, (char*)data + head, sizeof(int));
				//	head += sizeof(int);
				//
				//
				//	//	TEXT POS X
				//	memcpy(&currentText.PosX, (char*)data + head, sizeof(int));
				//	head += sizeof(int);
				//
				//
				//	//	TEXT POS Y
				//	memcpy(&currentText.PosY, (char*)data + head, sizeof(int));
				//	head += sizeof(int);
				//
				//	this->MainMenuTextVec.push_back(currentText);
				//}

				return ResourceHandler::InvokeReturn::SUCCESS| ResourceHandler::InvokeReturn::DEC_RAM;
			});

		}
		
	}
}



