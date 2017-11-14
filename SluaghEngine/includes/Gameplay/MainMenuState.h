#ifndef _SE_GAMEPLAY_MAIN_MENU_STATE_H
#define _SE_GAMEPLAY_MAIN_MENU_STATE_H
#include "IGameState.h"
#include <Gameplay\HUD_Parsing.h>
#include <Gameplay\HUD_Functionality.h>

namespace SE
{
	namespace Gameplay 
	{


		/**
		*
		* @brief The Main Menu Class
		*
		* @details class used for the main menu, more to be added later, inherits from Gamestate baseclass.
		*
		* @warning Not complete
		*
		* @sa IGameState.
		*
		**/
		class MainMenuState : public IGameState
		{

		public:

			MainMenuState();
			MainMenuState(Window::IWindow* Input, std::function<void()> shutDown);
			MainMenuState(Window::IWindow* Input);
			~MainMenuState();

			/**
			* @brief	The update loop for the Main menu process.
			*
			* @details	An update loop to be used during the Main menu phase, more info added later
			*
			* @param[in,out] passableInfo is a void pointer which lets us send information inbetween the states.
			*
			* @retval State return value to which other state to switch to when done.
			*
			* @warning Not fully implemented.
			*
			*/

			State Update(void* &passableInfo);
			HUDParser fileParser;
			IGameState::State CurrentState;

			

		private:

		protected:



		};

	}
}



#endif

