#ifndef SE_GAMEPLAY_SLOWED_CONDITION_LEAF_H
#define SE_GAMEPLAY_SLOWED_CONDITION_LEAF_H

#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class SlowedCondition : public IBehaviour
		{
		private:
			SlowedCondition() = delete;
		public:
			SlowedCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~SlowedCondition();


			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new SlowedCondition(enemyBlackboard, gameBlackboard);
			};
		};
	}
}
#endif
