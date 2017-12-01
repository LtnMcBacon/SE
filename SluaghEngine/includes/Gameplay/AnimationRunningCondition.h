#ifndef SE_GAMEPLAY_ANIMATION_RUNNING_CONDITION_H
#define SE_GAMEPLAY_ANIMATION_RUNNING_CONDITION_H
#include "IBehaviour.h"
#include <Utilz/GUID.h>
#include <vector>

namespace SE
{
	namespace Gameplay
	{
		class AnimationRunningCondition : public IBehaviour
		{
		private:
			AnimationRunningCondition() = delete;
			std::vector<Utilz::GUID> animationsToCheck;
			
		public:
			AnimationRunningCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, std::vector<Utilz::GUID> animations);
			~AnimationRunningCondition();


			Status Update() override;



			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new AnimationRunningCondition(enemyBlackboard, gameBlackboard, animationsToCheck);
			}

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}

#endif