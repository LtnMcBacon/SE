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
		};
		class HUDButtons
		{
		public:
			HUDButtons();
			~HUDButtons();


			
			void HoverButton(HUDInputCheck InputCheck);

		private:

		};

		
	}
}