#ifndef SE_GAMEPLAY_ATTACK_COLLDOWN_ZERO_CONDITION_H
#define SE_GAMPELAY_ATTACK_COLLDOWN_ZERO_CONDITION_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class AttackCooldownZeroCondition : public IBehaviour
		{
		private:
			AttackCooldownZeroCondition() = delete;

		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new AttackCooldownZeroCondition(gameBlackboard, enemyBlackboard);
			};

			AttackCooldownZeroCondition(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard);
			~AttackCooldownZeroCondition() override;
		};
	}
}

#endif