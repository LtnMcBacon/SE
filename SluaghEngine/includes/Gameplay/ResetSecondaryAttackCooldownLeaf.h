#ifndef SE_GAMEPLAY_RESET_SECONDARY_ATTACK_COLLDOWN_LEAF_H
#define SE_GAMEPLAY_RESET_SECONDARY_ATTACK_COLLDOWN_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class ResetSecondaryAttackCooldownLeaf : public IBehaviour
		{
		private:
			ResetSecondaryAttackCooldownLeaf() = delete;

		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new ResetSecondaryAttackCooldownLeaf(gameBlackboard, enemyBlackboard);
			};

			ResetSecondaryAttackCooldownLeaf(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard);
			~ResetSecondaryAttackCooldownLeaf() override;
		};
	}
}

#endif