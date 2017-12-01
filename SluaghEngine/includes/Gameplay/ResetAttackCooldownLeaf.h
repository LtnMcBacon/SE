#ifndef SE_GAMEPLAY_RESET_ATTACK_COLLDOWN_LEAF_H
#define SE_GAMPELAY_RESET_ATTACK_COLLDOWN_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class ResetAttackCooldownLeaf : public IBehaviour
		{
		private:
			ResetAttackCooldownLeaf() = delete;

		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new ResetAttackCooldownLeaf(gameBlackboard, enemyBlackboard);
			};

			ResetAttackCooldownLeaf(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard);
			~ResetAttackCooldownLeaf() override;

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}

#endif