#ifndef SE_GAMEPLAY_ANIMATION_LEAF_H
#define SE_GAMEPLAY_ANIMATION_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class CurrentAnimationAllowsBlendingCondition : public IBehaviour
		{
		private:
			CurrentAnimationAllowsBlendingCondition() = delete;
		public:
			CurrentAnimationAllowsBlendingCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~CurrentAnimationAllowsBlendingCondition();

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new CurrentAnimationAllowsBlendingCondition(enemyBlackboard, gameBlackboard);
			};
		};
	}
}







#endif