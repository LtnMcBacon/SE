#ifndef SE_GAMEPLAY_FLOW_FIELD_MOVEMENT_LEAF_H
#define SE_GAMEPLAY_FLOW_FIELD_MOVEMENT_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class FlowFieldMovementLeaf : public IBehaviour
		{
		private:
			FlowFieldMovementLeaf() = delete;
			int sample = 0;
			int sampleRate = 0;

			/**
			* @brief	Hinder collision during movement
			*
			* @details	This function checks if the units new position after a move will cause a collision.
			* If a move will cause a collision in x- or y-direction, then that direction-component will be nulled
			* before further computation is done.
			*
			* @param [in/out] xMov The x-component of the direction
			* @param [in/out] yMov The y-component of the direction
			*
			* @retval True Collision occoured
			* @retval False Collision didn't occour
			*
			*/
			bool CorrectCollision(float &xMov, float &yMov);

			void SampleFromMap(float &xMovementTot, float &yMovementTot);
		public:
			FlowFieldMovementLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~FlowFieldMovementLeaf();


			Status Update() override;



			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new FlowFieldMovementLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif
