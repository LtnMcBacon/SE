#ifndef SE_GAMEPLAY_ENEMY_BLACKBOARD_H
#define SE_GAMEPLAY_ENEMY_BLACKBOARD_H
#include "EventStructs.h"
#include "Room.h"

namespace SE
{
	namespace Gameplay
	{
		class EnemyUnit;


		/**
		*
		* @brief The struct holding all the data the enemies should know from the gamestate.
		*
		* @details Contains all the game-state data needed by the enemies to make decision with regard to the gameworld.
		*
		* @warning The clear function will NOT celar the data that will be set each frame.
		*
		* @sa EnemyBlackboard for data specific for an enemy.
		*
		**/
		struct EnemyBlackboard
		{
			EnemyBlackboard() { };

			/**
			* @brief	Clear data in the struct
			*
			* @details	Clear the data in the struct to default values. Will not change data that should be set each frame (for instance the owner)
			*
			*
			* @retval void No return
			* @warning Will NOT clear data that will be set each frame (for instance the owner)
			*
			*/
			void Clear()
			{

			};

			EnemyUnit* ownerPointer;
			/*Insert ptr to currently running task here*/

			float previousMovement[2] = {};
			float extents;
			bool invurnerable = false;
			bool onObstacle = false;
			bool checkedThisFrame = false;
			bool channeling = false;
			Boons activeCondition = Boons::CONDITIONAL_BOONS_NONE;
			float attackCooldown = 0.f;
			Room::DirectionToAdjacentRoom closestDoorDirection;

		};
	}
}
#endif
