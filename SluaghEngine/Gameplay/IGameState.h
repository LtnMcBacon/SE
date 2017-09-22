#ifndef _SE_GAMEPLAY_GAME_STATE_H
#define _SE_GAMEPLAY_GAME_STATE_H


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

		class GameState
		{
		public:
			GameState();
			~GameState();


			// An enum to decide whish state the game is in, mroe to be added if necessary.
			static enum  State
			{
				MENU_STATE,
				GAME_STATE,
				END_STATE
			};

			// perhaps a pointer to decide states when switching between objects?
			// or a bunch of subclasses objects.

		private:

			void InitializeState();
			void ChangeState();
			void UpdateState();
			void EndState();

		protected:
		};











	}
}




#endif 