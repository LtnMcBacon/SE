#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <Utilz\GUID.h>
#include <Core\IGUIManager.h>
#include <functional>
#include <Gameplay\HUD_Functionality.h>

namespace SE
{
	namespace Gameplay
	{
		class HUDParser
		{
		public:

			HUDButtons GUIButtons;
			
			int entityIndex;
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
				int skillIndex;

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
			std::vector<HUDElement> StaticGuiVec;
			std::vector<SE::Core::Entity> texEntityVec;

			std::vector <HUDElement> ButtonVector;
			std::vector<HUDText> MainMenuTextVec;

			std::vector<HUDElement> skillButtonVec;
			std::vector<HUDText> skillTextVec;

			std::vector<HUDElement> perkButtonVec;
			std::vector<HUDText> perkTextVec;

			std::vector<HUDElement> optionsMenu;
			std::vector<HUDElement> optionButtons;

			HUDParser();
			~HUDParser();
			void ParseFiles(Utilz::GUID fileName);
			void ParseSkillButtons(Utilz::GUID fileName);
			void ParsePerks(Utilz::GUID fileName);
			void ParseOptionMenu(Utilz::GUID fileName);
			void InitiateTextures();
			
			
			
			Core::IGUIManager::CreateInfo GuiManager;


		private:

		};

		
	}
	
}
