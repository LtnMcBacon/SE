#ifndef SE_GAMEPLAY_STUNNED_CONDITION_LEAF_H
#define SE_GAMEPLAY_STUNNED_CONDITION_LEAF_H

#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class StunnedCondition : public IBehaviour
		{
		private:
			StunnedCondition() = delete;
		public:
			StunnedCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~StunnedCondition();


			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new StunnedCondition(enemyBlackboard, gameBlackboard);
			};
		};
	}
}
#endif
