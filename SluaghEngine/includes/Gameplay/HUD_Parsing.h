#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <Utilz\GUID.h>


namespace SE
{
	namespace Gameplay
	{
		class HUDParser
		{
		public:

			struct HUDElement
			{
				std::string rectName;
				std::string textName;
				int Width;
				int Height;
				int PositionX;
				int PositionY;
				int Opacity;

				bool Btn;
				std::string hoverTex;
				std::string PressTex;
				int layerDepth;
			};

			struct HUDText
			{
				std::string TextName;
				std::string TextContent;
				int Width;
				int Height;
				int PosX;
				int PosY;
			};
			std::vector<HUDElement> MainMenuElmVec;
			std::vector<HUDText> MainMenuTextVec;

			HUDParser();
			~HUDParser();
			void ParseFiles(Utilz::GUID fileName);
			void InitiateTextures();
			void checkPressed(bool pressed, int mousePosX, int mousePosY);

			void HoverButton(bool pressed, int mousePosX, int mousePosY, HUDElement HUDButton);
			void texHovered();
			void texPressed();
			void texIdle();

			Core::IGUIManager::CreateInfo GuiManager;
		private:

		};

		
	}
	
}