#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <Utilz\GUID.h>
#include <Core\IGUIManager.h>
#include <functional>

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
				int EntityID;
				int EntityIndex;
				bool Btn;
				std::string hoverTex;
				std::string PressTex;
				int layerDepth;

				std::function<void()> bindButton;

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
			std::vector<SE::Core::Entity> texEntityVec;
			std::vector<HUDText> MainMenuTextVec;

			HUDParser();
			~HUDParser();
			void ParseFiles(Utilz::GUID fileName);
			void InitiateTextures();
			void checkPressed(bool pressed, int mousePosX, int mousePosY);

			void HoverButton(bool pressed, int mousePosX, int mousePosY, HUDElement HUDButton);
			void texHovered( HUDElement HUDButton);
			void texPressed(HUDElement HUDButton);
			void texIdle(HUDElement HUDButton);

			void SetFunctionOnPress(std::string identifier, std::function<void()> func);
			

			Core::IGUIManager::CreateInfo GuiManager;


		private:

		};

		
	}
	
}
