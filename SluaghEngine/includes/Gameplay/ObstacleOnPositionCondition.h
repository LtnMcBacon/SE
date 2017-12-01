#ifndef SE_GAMEPLAY_OBSTACLE_ON_POSITION_LEAF_H
#define SE_GAMEPLAY_OBSTACLE_ON_POSITION_LEAF_H

#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class ObstacleOnPositionCondition : public IBehaviour
		{
		private:
			ObstacleOnPositionCondition() = delete;
		public:
			ObstacleOnPositionCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~ObstacleOnPositionCondition();


			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new ObstacleOnPositionCondition(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}
#endif
