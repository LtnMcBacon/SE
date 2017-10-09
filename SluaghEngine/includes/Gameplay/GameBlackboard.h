#ifndef SE_GAMEPLAY_GAME_BLACKBOARD_H
#define SE_GAMEPLAY_GAME_BLACKBOARD_H

namespace SE
{
	namespace Gameplay
	{
		class FlowField;
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
		struct GameBlackboard
		{
			GameBlackboard() { };

			/**
			* @brief	Clear data in the struct
			*
			* @details	Clear the data in the struct to default values. Will not change data that should be set each frame (for instance player position)
			*
			*
			* @retval void No return
			* @warning Will NOT clear data that will be set each frame (for instance player position)
			*
			*/
			void Clear()
			{
					
			};

			float playerPositionX = 0.f;
			float playerPositionY = 0.f;
			float playerMovementX = 0.f;
			float playerMovementY = 0.f;
			float playerHealth = 0.f;
			float deltaTime = 0.f;

			FlowField* roomFlowField;
			/*Player resistances here*/
			/*Player attack data*/


		};
	}
}
#endif
