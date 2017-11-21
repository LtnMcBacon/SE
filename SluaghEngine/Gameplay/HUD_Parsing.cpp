#include <HUD_Parsing.h>
#include "CoreInit.h"
#include <Profiler.h>
using namespace std;


namespace SE
{
	namespace Gameplay
	{
		HUDParser::HUDParser()
		{
			StartProfile;
			std::function<void()> test = std::bind(&HUDParser::InitiateTextures, this);
			test();
			this->InitiateTextures();
			ProfileReturnVoid;
		}

		HUDParser::~HUDParser()
		{
			StartProfile;
			for (auto& entities: texEntityVec)
			{
				CoreInit::managers.guiManager->ToggleRenderableTexture(entities, false);
				CoreInit::managers.entityManager->Destroy(entities);
			}
			ProfileReturnVoid;
		}

		void HUDParser::ParseFiles(Utilz::GUID fileName)
		{
			StartProfile;

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

						this->StaticGuiVec.push_back(CurrentElm);
					}
					
				}

				

				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
			});
			ProfileReturnVoid;
		}

		void HUDParser::InitiateTextures()
		{
			StartProfile;
			
			for (auto& HUDElement : StaticGuiVec)
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
			ProfileReturnVoid;
		}

		void HUDParser::ParseSkillButtons(Utilz::GUID fileName)
		{
			StartProfile;
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

				

				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
			});
			ProfileReturnVoid;
		}

		void HUDParser::ParsePerks(Utilz::GUID fileName)
		{
			StartProfile;
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

				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
			});
			ProfileReturnVoid;
		}

		void HUDParser::ParseOptionalButtons(Utilz::GUID fileName)
		{

			StartProfile;
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
						OptionalButtons.push_back(CurrentElm);
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
			ProfileReturnVoid;

		}
		
	}
}



