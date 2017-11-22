#ifndef SE_GAMEPLAY_TICK_DOWN_SECONDARY_ATTACK_COOLDOWN_LEAF_H
#define SE_GAMEPLAY_TICK_DOWN_SECONDARY_ATTACK_COOLDOWN_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class TickDownSecondaryAttackCooldownLeaf : public IBehaviour
		{
		private:
			TickDownSecondaryAttackCooldownLeaf() = delete;

		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new TickDownSecondaryAttackCooldownLeaf(gameBlackboard, enemyBlackboard);
			};

			TickDownSecondaryAttackCooldownLeaf(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard);
			~TickDownSecondaryAttackCooldownLeaf() override;
		};
	}
}

#endif