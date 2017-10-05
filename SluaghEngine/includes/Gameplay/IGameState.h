#ifndef _SE_GAMEPLAY_GAME_STATE_H
#define _SE_GAMEPLAY_GAME_STATE_H
#include <window/IWindow.h>

namespace SE
{
	namespace Gameplay
	{

		/**
		*
		* @brief The base class for gamestates
		*
		* @details This class is used as the base class for the gamestates in the game, more details will be added later on
		* 
		*
		* @warning Not fully implemented.
		**/

		class IGameState
		{
		public:
			IGameState();
			IGameState(void* state);
			~IGameState();


			// An enum to decide whish state the game is in, mroe to be added if necessary.
			 enum  State
			{
				MAIN_MENU_STATE = 0,
				PLAY_STATE = 1,
				GAME_OVER_STATE = 2,
				CHARACTER_CREATION_STATE = 3,
				PAUSE_STATE = 4
			};

			// perhaps a pointer to decide states when switching between objects?
			// or a bunch of subclasses objects.

			virtual State Update(void*&) = 0;
		private:

			void InitializeState();
			void ChangeState();
			void UpdateState();
			void EndState();


		protected:
			Window::IWindow* Input;

		};











	}
}




#endif 