#ifndef _SE_GAMEPLAY_GAME_OVER_STATE_H
#define _SE_GAMEPLAY_GAME_OVER_STATE_H
#include "IGameState.h"

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The Game Over Class
		*
		* @details class used for the Game over state, more to be added later, inherits from Gamestate baseclass.
		*
		* @warning Not complete
		*
		* @sa GameState.
		*
		**/
		class GameOverState : public GameState
		{
		public:
			GameOverState();
			~GameOverState();

			/**
			* @brief	The update loop for the Game over process.
			*
			* @details	An update loop to be used during the Game over phase, more info added later
			*
			* @param[in] Input is a pointer to the Window class which lets us use user input within these update loops
			* @param[in,out] passableInfo is a void pointer which lets us send information inbetween the states.
			*
			* @retval State return value to which other state to switch to when done.
			*
			* @warning Not fully implemented.
			*
			*/
			State Update(Window::IWindow* Input,void* &passableInfo);
		private:

		protected:

		};

		
	}
}

#endif 

