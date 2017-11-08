#ifndef SE_GAMEPLAY_ANIMATION_RUNNING_CONDITION_H
#define SE_GAMEPLAY_ANIMATION_RUNNING_CONDITION_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class AnimationRunningCondition : public IBehaviour
		{
		private:
			AnimationRunningCondition() = delete;
			
		public:
			AnimationRunningCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~AnimationRunningCondition();


			Status Update() override;



			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new AnimationRunningCondition(enemyBlackboard, gameBlackboard);
			};
		};
	}
}

#endif