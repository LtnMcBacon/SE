#pragma once
#include <string>
#include <window/IWindow.h>



namespace SE
{
	namespace Gameplay
	{

		struct HUDInputCheck
		{
			int mousePosX;
			int mousePosY;
			int posX;
			int posY;
			int width;
			int height;
			bool pressed;
			std::string pressTex;
			std::string hoverTex;
			std::string idleTex;
			double layerDepth;
		};
		class HUDButtons
		{
		public:
			HUDButtons();
			~HUDButtons();


			
			void HoverButton(HUDInputCheck InputCheck );
			void texHovered(HUDInputCheck InputCheck);
			void texPressed(HUDInputCheck InputCheck);
			void texIdle(HUDInputCheck InputCheck);

		private:

		};

		
	}
}