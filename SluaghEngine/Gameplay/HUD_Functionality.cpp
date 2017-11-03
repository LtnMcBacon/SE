#include "HUD_Functionality.h"

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

		void HUDButtons::HoverButton(HUDInputCheck InputCheck )
		{
			
			bool inside = false;

			if (InputCheck.mousePosX < InputCheck.posX + InputCheck.width && InputCheck.mousePosX > InputCheck.posX)
			{
				if (InputCheck.mousePosY < InputCheck.posY + InputCheck.height && InputCheck.mousePosY > InputCheck.posY)
				{
					inside = true;
				}
			}

			if (inside)
			{
				if (InputCheck.pressed)
				{

				}
				else
				{

				}
			}

		}

	}
}

