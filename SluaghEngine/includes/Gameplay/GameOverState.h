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
		* @sa IGameState.
		*
		**/
		class GameOverState : public IGameState
		{
		public:
			GameOverState();
			GameOverState(Window::IWindow* Input);
			~GameOverState();

			/**
			* @brief	The update loop for the Game over process.
			*
			* @details	An update loop to be used during the Game over phase, more info added later
			*
			* @param[in,out] passableInfo is a void pointer which lets us send information inbetween the states.
			*
			* @retval State return value to which other state to switch to when done.
			*
			* @warning Not fully implemented.
			*
			*/
			State Update(void* &passableInfo);
		private:

		protected:

		};

		
	}
}

#endif 

