#pragma once
#include <string>
#include <window/IWindow.h>
#include <vector>
#include <Core\IGUIManager.h>
#include <functional>

namespace SE
{
	namespace Gameplay
	{
		
		class HUDButtons
		{
		public:
			HUDButtons();
			~HUDButtons();

			// struct for buttons
			struct ButtonElement
			{
				std::string rectName = "";
				std::string textName = "";
				int Width = 0;
				int Height = 0;
				int PositionX = 0;
				int PositionY = 0;
				int Opacity = 1;
				int EntityID = 0;
				int EntityIndex = 0;
				std::string hoverTex = "";
				std::string PressTex = "";
				int layerDepth = 0;
				std::string buttonText = "";
				int textEntityID = 0;
				int textEntityIndex = 0;
				unsigned short skillDesc[8];
				bool skillButton = false;
				std::string skillName;
				std::string perkName;
				std::function<void()> bindButton;
			};

			bool wasHovering = false;
			size_t additionalHeight = 0;
			size_t additionalWidth = 0;
			// vector to store buttons
			std::vector<ButtonElement> Buttons;
			std::vector<std::string> ButtonsToDelete;
			// vector to store buttons drawing entities
			std::vector<SE::Core::Entity> ButtonEntityVec;
			SE::Core::Entity skillDescEntity;
			
			std::vector<SE::Core::Entity> SkillNPerkEntityVec;

			// counter for every entity drawn
			int entityIndex;

			Core::IGUIManager::CreateInfo ButtonGuiManager;

			//void CreateButton(int posX, int posY, int width, int height, int layerDepth, std::string name, std::function<void()> func, std::string textName = "checkerboard.png", std::string buttonText = "");
			void CreateButton(int posX, int posY, int width, int height, int layerDepth, std::string name, std::function<void()> func, std::string textName = "checkerboard.png", std::string hoverTex = "checkerboard.png", std::string PressTex = "checkerboard.png", std::string buttonText = "");
			void CreateButton(int posX, int posY, int width, int height, int layerDepth, std::string name, std::function<void()> func, bool skill,std::string perkName, std::string textName = "checkerboard.png", std::string hoverTex = "checkerboard.png", std::string PressTex = "checkerboard.png", std::string buttonText = "");
			void CreateButton(int posX, int posY, int width, int height, int layerDepth, std::string name, std::function<void()> func, unsigned short int skillDesc[],std::string skillName, std::string textName = "checkerboard.png", std::string hoverTex = "checkerboard.png", std::string PressTex = "checkerboard.png", std::string buttonText = "");

			std::string printSkillDesc(ButtonElement button);

			void DrawButtons();
			void DrawButtonText(ButtonElement button);
			void ButtonHover(int mousePosX, int mousePosY,bool pressed, bool released);
			void DeleteButtons();
			void HideButtons();
			void ShowButtons();
			void CalculateScreenPositions();
			void deleteSkillPerkBtns();

		};
	}
}