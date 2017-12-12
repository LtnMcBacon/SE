#ifndef SE_GAMEPLAY_ROOTED_CONDITION_LEAF_H
#define SE_GAMEPLAY_ROOTED_CONDITION_LEAF_H

#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class RootedCondition : public IBehaviour
		{
		private:
			RootedCondition() = delete;
		public:
			RootedCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~RootedCondition();


			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new RootedCondition(enemyBlackboard, gameBlackboard);
			};
		};
	}
}
#endif
