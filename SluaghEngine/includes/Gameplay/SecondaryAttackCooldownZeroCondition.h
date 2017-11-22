#ifndef SE_GAMPELAY_SECONDARY_ATTACK_COLLDOWN_ZERO_CONDITION_H
#define SE_GAMPELAY_SECONDARY_ATTACK_COLLDOWN_ZERO_CONDITION_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class SecondaryAttackCooldownZeroCondition : public IBehaviour
		{
		private:
			SecondaryAttackCooldownZeroCondition() = delete;

		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new SecondaryAttackCooldownZeroCondition(gameBlackboard, enemyBlackboard);
			};

			SecondaryAttackCooldownZeroCondition(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard);
			~SecondaryAttackCooldownZeroCondition() override;
		};
	}
}

#endif