#ifndef _SE_GAMEPLAY_CHARACTER_CREATION_STATE_H
#define _SE_GAMEPLAY_CHARACTER_CREATION_STATE_H
#include "IGameState.h"
#include <Gameplay\Skill.h>

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The Character Creation Class
		*
		* @details class used for the character creation process, more to be added later, inherits from Gamestate baseclass.
		*
		* @warning Not complete
		*
		* @sa IGameState.
		*
		**/

		class CharacterCreationState : public IGameState
		{

		public:
			CharacterCreationState();
			CharacterCreationState(Window::IWindow* Input);
			~CharacterCreationState();


			/**
			* @brief	The Update loop for the Character creation phase.
			*
			* @details	An update loop to be used during the Character Creation phase, more info added later
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
