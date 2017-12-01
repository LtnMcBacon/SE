#ifndef SE_GAMEPLAY_CURRENT_ANIMATION_ALLOWS_BLENDING_CONDITION_H
#define SE_GAMEPLAY_CURRENT_ANIMATION_ALLOWS_BLENDING_CONDITION_H
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

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}







#endif