#ifndef _SE_GAMEPLAY_PAUS_STATE_H
#define _SE_GAMEPLAY_PAUS_STATE_H
#include "IGameState.h"
#include <Gameplay\HUD_Parsing.h>
#include <Gameplay\HUD_Functionality.h>

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The Pause menu Class
		*
		* @details class used for the pause menu, more to be added later, inherits from Gamestate baseclass.
		*
		* @warning Not complete
		*
		* @sa IGameState.
		*
		**/
		class PauseState : public IGameState
		{

		public:
			PauseState();
			PauseState(Window::IWindow* Input);
			~PauseState();
			/**
			* @brief	The update loop for the pause menu process.
			*
			* @details	An update loop to be used during the pause menu phase, more info added later
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


