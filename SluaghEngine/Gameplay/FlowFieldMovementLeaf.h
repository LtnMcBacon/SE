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
		public:
			FlowFieldMovementLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~FlowFieldMovementLeaf();

			Status Update() override;

			bool CorrectCollision(float &xMov, float &yMov);

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new FlowFieldMovementLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif
