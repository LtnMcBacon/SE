#ifndef _SE_GAMEPLAY_GAME_UNIT_H_
#define _SE_GAMEPLAY_GAME_UNIT_H_
#include "Core/Entity.h"

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The base class for all units in the game.
		*
		* @details This class is used as the base for the Player and Enemy classes. Here, information that is shared between the two
		* classes are placed, so to minimized duplicated code data. NOTE: So far, no need for virtual functions has been found.
		*
		* @warning Not fully implemented, due to many decisions not being done.
		*
		* @sa Enemy, Player.
		*
		**/
		class GameUnit
		{
		public:
			GameUnit();
			~GameUnit();
		protected:
			Core::Entity unitEntity = {};
			//Life. Float needed?
			//Transforms and the like will be created inside the EnemyFactory, and outside of this class. During the sprint, this will most likely be done in the playstate
			//Functions to become damaged?
			//Weapons? Needed here?

		public:
			inline const Core::Entity& GetEntity() const
			{
				return unitEntity;
			}
			
		};
	}
}




#endif
